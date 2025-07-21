/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/21 18:06:46 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time_stamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	has_anyone_died(philo_t *philo)
{
	int	dead;

	pthread_mutex_lock(&philo->shared_data->check_dead);
	dead = philo->shared_data->someone_died;
	pthread_mutex_unlock(&philo->shared_data->check_dead);
	return (dead);
}

static void	print_message(philo_t *philo, long time, char *msg)
{
	pthread_mutex_lock(&philo->shared_data->write);
	if (!has_anyone_died(philo))
		printf("%ld %d %s", time, philo->id, msg);
	pthread_mutex_unlock(&philo->shared_data->write);
	
}

// FUNCION QUE HACE USO DE USLEEP PERO CON COMPROBACION DE MUERTE CADA VEZ
void	usleep_precise(long time, philo_t *philo)
{
	long	start;
	
	start = get_time_stamp();
	while (get_time_stamp() - start < time)
	{
		if(has_anyone_died(philo))
		{
			break ;
		}
		usleep(1000);
	}
	return ;
}

void *monitor_function(void *arg)
{
	philo_monitor_args *args = (philo_monitor_args *)arg;
	philo_t *philosophers = args->philosophers;
	long num = args->number_of_philosophers;

	int i;
	while (1)
	{
		i = 0;
		while (i < num && philosophers[i].id != 0)
		{
			pthread_mutex_lock(&philosophers[i].shared_data->meal_mutex);
			// printf("Valor de time_stamp %ld\n",get_time_stamp());
			// printf("Valor de time of last meal %ld\n", philosophers[i].time_of_last_meal);
			// printf("Valor de time to die %ld\n", philosophers[i].time_to_die);
			long current_time = get_time_stamp();
			long time_since_last_meal = current_time - philosophers[i].time_of_last_meal;
			if (time_since_last_meal > philosophers[i].time_to_die)
			{
				pthread_mutex_unlock(&philosophers[i].shared_data->meal_mutex);
				pthread_mutex_lock(&philosophers[i].shared_data->check_dead);
				if (!philosophers[i].shared_data->someone_died)
				{
					philosophers[i].shared_data->someone_died = 1;
					printf("%ld %d died\n", get_time_stamp()- philosophers[i].time_of_last_meal, philosophers[i].id);
				}
				pthread_mutex_unlock(&philosophers[i].shared_data->check_dead);
				return NULL;
			}
			pthread_mutex_unlock(&philosophers[i].shared_data->meal_mutex);
			i++;
		}
		usleep(1000); // 1 ms
	}
	return NULL;
}


void	*thread_function(void *arg)
{
	philo_t *philo = (philo_t *)arg;
	long start_time;

	start_time = philo->shared_data->start_time;
	pthread_mutex_lock(&philo->shared_data->meal_mutex);
	philo->time_of_last_meal = get_time_stamp();
	pthread_mutex_unlock(&philo->shared_data->meal_mutex);
	if (philo->id % 2 != 0)
	{
		usleep_precise(1, philo);
	}
	while (!has_anyone_died(philo))
	{
		//COGEN LOS TENEDORES
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a fork\n");
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a fork\n");
		}
		else
		{
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a fork\n");
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, get_time_stamp() - start_time, "has taken a fork\n");
		}
		//LOS FILOSOFOS EMPIEZAN A COMER
		pthread_mutex_lock(&philo->shared_data->meal_mutex);
		philo->time_of_last_meal = get_time_stamp();
		//printf("Time of last meal %ld\n", get_time_stamp() - start_time);
		print_message(philo, get_time_stamp() - start_time, "is eating\n");	
		pthread_mutex_unlock(&philo->shared_data->meal_mutex);
		usleep_precise(philo->time_to_eat, philo);
		philo->meals_eaten++;
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		//LOS FILOSOFOS TERMINAN DE COMER
		//LOS FILOSOFOS EMPIEZAN A DORMIR
		print_message(philo, get_time_stamp() - start_time, "is sleeping\n");
		usleep_precise(philo->time_to_sleep, philo);
		//LOS FILOSOFOS TERMINAN DE DORMIR
		//LOS FILOSOFOS EMPIEZAN A PENSAR EL RESTO DELE TIEMPO QUE LE QUEDA
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

void	initialize_mutexes(shared_data_t *shared)
{
	pthread_mutex_init(&shared->dead, NULL);
	pthread_mutex_init(&shared->check_dead, NULL);
	pthread_mutex_init(&shared->write, NULL);
	pthread_mutex_init(&shared->meal_mutex, NULL);
	shared->someone_died = 0;
}


int	initialize_forks(long	number_of_philosophers,
philo_t	*philosophers, pthread_mutex_t *forks)
{
	int	i;
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].id = i + 1;
		if (pthread_mutex_init(&forks[i], NULL) != 0)
		{
			perror("Error inicializando el mutex");
			return (1);
		}
		i++;
	}
	return (0);
}

void	initialize_philosophers(
int argc, char **argv,
long number_of_philosophers,
philo_t *philosophers, pthread_mutex_t *forks,
shared_data_t *shared)
{	
	int	i;

	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].time_to_die = ft_atol(argv[2]);
		philosophers[i].time_to_eat = ft_atol(argv[3]);
		philosophers[i].time_to_sleep = ft_atol(argv[4]);
		philosophers[i].number_of_times_each_philosopher_must_eat = 0;
		if (argc == 6)
			philosophers[i].number_of_times_each_philosopher_must_eat = ft_atol(argv[5]);
		philosophers[i].meals_eaten = 0;
		philosophers[i].time_to_think = philosophers[i].time_to_die - (philosophers[i].time_to_sleep + philosophers[i].time_to_eat);
		philosophers[i].left_fork = &forks[i];
		philosophers[i].right_fork = &forks[(i + 1) %  number_of_philosophers];
		philosophers[i].shared_data = shared;
		i++;
	}
}

int main(int argc, char **argv)
{
	philo_t			*philosophers;
	pthread_t		*thread;
	pthread_t		monitor;
	pthread_mutex_t	*forks;
	shared_data_t	shared;
	int				i;
	long			number_of_philosophers;

	number_of_philosophers = ft_atol(argv[1]);
	shared.someone_died = 0;
	shared.start_time = get_time_stamp();
	philosophers = malloc(sizeof(philo_t) * number_of_philosophers);
	thread = malloc(sizeof(pthread_t) * number_of_philosophers);
	forks = malloc(sizeof(pthread_mutex_t) * number_of_philosophers);
	if (!philosophers || !thread || !forks)
	{
		perror("Error al reservar memoria");
		return (1);
	}
	initialize_mutexes(&shared);
	initialize_forks(number_of_philosophers, philosophers, forks);
	initialize_philosophers(argc, argv, number_of_philosophers, philosophers,forks, &shared);
	create_threads(philosophers, thread, number_of_philosophers);
	philo_monitor_args *args = malloc(sizeof(philo_monitor_args));
	args->philosophers = philosophers;
	args->number_of_philosophers = number_of_philosophers;
	if (pthread_create(&monitor, NULL, &monitor_function, args) != 0)
	{
		perror("Error creando el monitor");
		return (1);
	}
	pthread_join(monitor, NULL);
	end_threads(number_of_philosophers, thread);
	pthread_mutex_destroy(&shared.dead);
	pthread_mutex_destroy(&shared.check_dead);
	pthread_mutex_destroy(&shared.meal_mutex);
	pthread_mutex_destroy(&shared.write);

	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_mutex_destroy(&forks[i]);
		i++;
	}
	printf("Terminan aquí\n");
}
