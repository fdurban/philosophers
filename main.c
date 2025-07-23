/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernando <fernando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/23 03:23:29 by fernando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	print_message(philo_t *philo, long time, char *msg)
{
	pthread_mutex_lock(&philo->shared_data->write);
	if (!has_anyone_died(philo))
		printf("%ld %d %s", time, philo->id, msg);
	pthread_mutex_unlock(&philo->shared_data->write);
	
}

void	usleep_precise(long time, philo_t *philo)
{
	long	start;
	long	now;
	long	elapsed;
	long	remaining;

	start = get_time_stamp();
	while (1)
	{
		now = get_time_stamp();
		elapsed = now - start;
		if (elapsed >= time || has_anyone_died(philo))
			break ;
		remaining = time - elapsed;
		if (remaining > 1000)
			usleep(500);
		else if (remaining > 100)
			usleep(100);
		else
			usleep(10);
	}
}

void *monitor_function(void *arg)
{
	philo_monitor_args *args = (philo_monitor_args *)arg;
	philo_t *philosophers = args->philosophers;
	long num = args->number_of_philosophers;

	int i;
	int started = 0;
	while (!started)
	{
		pthread_mutex_lock(&philosophers->shared_data->start_mutex);
		started = philosophers->shared_data->start_flag;
		pthread_mutex_unlock(&philosophers->shared_data->start_mutex);
		if (!started)
			usleep(50);
	}
	while (1)
	{
		i = 0;
		while (i < num && philosophers[i].id != 0)
		{
			pthread_mutex_lock(&philosophers[i].meal_mutex);
			long current_time = get_time_stamp();
			long time_since_last_meal = current_time - philosophers[i].time_of_last_meal;
			if (time_since_last_meal > philosophers[i].time_to_die)
			{
				pthread_mutex_unlock(&philosophers[i].meal_mutex);
				pthread_mutex_lock(&philosophers[i].shared_data->check_dead);
				if (!philosophers[i].shared_data->someone_died)
				{
					philosophers[i].shared_data->someone_died = 1;
					printf("%ld %d died\n", get_time_stamp() - philosophers[i].shared_data->start_time, philosophers[i].id);
				}
				pthread_mutex_unlock(&philosophers[i].shared_data->check_dead);
				return NULL;
			}
			pthread_mutex_unlock(&philosophers[i].meal_mutex);
			i++;
		}
		usleep(100);
	}
	return NULL;
}


void	*thread_function(void *arg)
{
	philo_t *philo = (philo_t *)arg;
	long start_time;

	int started = 0;
	while (!started)
	{
		pthread_mutex_lock(&philo->shared_data->start_mutex);
		started = philo->shared_data->start_flag;
		pthread_mutex_unlock(&philo->shared_data->start_mutex);
		if (!started)
			usleep(50);
	}

	pthread_mutex_lock(&philo->meal_mutex);
	philo->time_of_last_meal = get_time_stamp();
	pthread_mutex_unlock(&philo->meal_mutex);
	start_time = philo->shared_data->start_time;
		if (philo->id % 2 != 0)
		{
			usleep_precise((philo->time_to_eat/2), philo);
		}
	while (!has_anyone_died(philo))
	{
		if (philo->shared_data->meals_required != -1 && 
			philo->meals_eaten >= philo->shared_data->meals_required)
		break;
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a left fork\n");
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a right fork\n");
		}
		else
		{
			usleep(1000);
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a right fork\n");
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a left fork\n");
		}
		pthread_mutex_lock(&philo->meal_mutex);
		philo->time_of_last_meal = get_time_stamp();
		print_message(philo, get_time_stamp() - start_time, "is eating\n");	
		pthread_mutex_unlock(&philo->meal_mutex);
		usleep_precise(philo->time_to_eat, philo);
		philo->meals_eaten++;
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		print_message(philo, get_time_stamp() - start_time, "is sleeping\n");
		usleep_precise(philo->time_to_sleep, philo);
		print_message(philo, get_time_stamp() - start_time, "is thinking\n");
	}
	return (NULL);
}

int	create_threads(philo_t	*philosophers, pthread_t	*thread, long	number_of_philosophers)
{
	int	i;
	
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_create(&thread[i], NULL, &thread_function, &philosophers[i]) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	return (0);
}

int	end_threads(long number_of_philosophers, pthread_t *thread)
{
	int	i;

	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_join(thread[i], NULL) != 0)
		{
			perror("Error waiting for thread to finish");
			return (1);
		}
		i++;
	}
	return (0);
}

void	clean_up(t_simulation	*simulation)
{
	int i;

	i = 0;
	while (i < simulation->number_of_philosophers)
	{
		pthread_mutex_destroy(&simulation->forks[i]);
		pthread_mutex_destroy(&simulation->philosophers[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&simulation->shared.check_dead);
	pthread_mutex_destroy(&simulation->shared.write);
	pthread_mutex_destroy(&simulation->shared.start_mutex);
	free(simulation->philosophers);
	free(simulation->threads);
	free(simulation->forks);
}

void create_monitor_thread(t_simulation *sim)
{
	philo_monitor_args *args = malloc(sizeof(philo_monitor_args));
	args->philosophers = sim->philosophers;
	args->number_of_philosophers = sim->number_of_philosophers;
	if (pthread_create(&sim->monitor, NULL, monitor_function, args) != 0)
	{
		perror("Error creando el monitor");
		clean_up(sim);
		exit(1);
	}
}

int	parse_args(int argc, char **argv, t_simulation *sim)
{
	sim->number_of_philosophers = ft_atol(argv[1]);
	sim->time_to_die = ft_atol(argv[2]);
	sim->time_to_eat = ft_atol(argv[3]);
	sim->time_to_sleep = ft_atol(argv[4]);

	if (argc == 6)
		sim->number_of_times_each_philosopher_must_eat = ft_atol(argv[5]);
	else
		sim->number_of_times_each_philosopher_must_eat = -1;

	if (sim->number_of_philosophers <= 0 || sim->time_to_die < 0 || 
		sim->time_to_eat < 0 || sim->time_to_sleep < 0 || 
		(argc == 6 && sim->number_of_times_each_philosopher_must_eat < 0))
	{
		printf("Error: argumentos inválidos\n");
		return (0);
	}
	return (1);
}


int main(int argc, char **argv)
{
	if (argc != 5 && argc != 6)
	{
		printf("Not the right number of arguments");;
		return (1);
	}
	t_simulation	simulation;

	simulation.number_of_philosophers = ft_atol(argv[1]);
	simulation.shared.someone_died = 0;
	simulation.philosophers = malloc(sizeof(philo_t) * simulation.number_of_philosophers);
	simulation.threads = malloc(sizeof(pthread_t) * simulation.number_of_philosophers);
	simulation.forks = malloc(sizeof(pthread_mutex_t) * simulation.number_of_philosophers);
	simulation.argc = argc;
	simulation.argv = argv;
	simulation.shared.start_flag = 0;
	parse_args(argc, argv, &simulation);
	simulation.shared.meals_required = simulation.number_of_times_each_philosopher_must_eat;
	if (!simulation.philosophers || !simulation.threads|| !simulation.forks)
	{
		perror("Error al reservar memoria");
		return (1);
	}
	initialize_mutexes(&simulation.shared);
	initialize_forks(&simulation);
	initialize_philosophers(&simulation);
	create_threads(simulation.philosophers, simulation.threads, simulation.number_of_philosophers);
	create_monitor_thread(&simulation);
	pthread_mutex_lock(&simulation.shared.start_mutex);
	simulation.shared.start_time = get_time_stamp();
	simulation.shared.start_flag = 1;
	pthread_mutex_unlock(&simulation.shared.start_mutex);
	pthread_join(simulation.monitor, NULL);
	end_threads(simulation.number_of_philosophers, simulation.threads);
	clean_up(&simulation);
	printf("Terminan aquí\n");
}
