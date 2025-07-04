/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/06/27 16:07:57 by fdurban-         ###   ########.fr       */
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
		usleep(1);
	}
	return ;
}

void *monitor_function(void *arg)
{
	philo_t *philosophers = (philo_t *)arg;
	int i;
	
	while (1)
	{
		i = 0;
		while (i < PHILO_MAX && philosophers[i].id != 0)
		{
			pthread_mutex_lock(&philosophers[i].shared_data->meal_mutex);
			if (get_time_stamp() - philosophers[i].time_of_last_meal > philosophers[i].time_to_die)
			{
				pthread_mutex_unlock(&philosophers[i].shared_data->meal_mutex);
				pthread_mutex_lock(&philosophers[i].shared_data->check_dead);
				if (!philosophers[i].shared_data->someone_died)
				{
					philosophers[i].shared_data->someone_died = 1;
					printf("%ld %d died\n", get_time_stamp(), philosophers[i].id);
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
	long start_time = get_time_stamp();

	if (philo->id == 1)
		usleep_precise(philo->time_to_die + 100, philo);
	pthread_mutex_lock(&philo->shared_data->meal_mutex);
	philo->time_of_last_meal = get_time_stamp();
	pthread_mutex_unlock(&philo->shared_data->meal_mutex);
	if (philo->id % 2 != 0)
	{
		usleep_precise(philo->time_to_sleep, philo);
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

int	create_threads(philo_t	philosophers[PHILO_MAX], pthread_t	thread[PHILO_MAX], long	number_of_philosophers)
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

int	end_threads(long number_of_philosophers, pthread_t thread[PHILO_MAX])
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
philo_t	philosophers[PHILO_MAX], pthread_mutex_t forks[PHILO_MAX])
{
	int	i;
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].id = i + 1;
		printf("valor de id es %d\n", philosophers[i].id);
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
philo_t philosophers[PHILO_MAX], pthread_mutex_t forks[PHILO_MAX],
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
	philo_t			philosophers[PHILO_MAX];
	pthread_t		thread[PHILO_MAX];
	pthread_t		monitor;
	pthread_mutex_t	forks[PHILO_MAX];
	shared_data_t	shared;
	int				i;
	long			number_of_philosophers;

	number_of_philosophers = ft_atol(argv[1]);
	shared.someone_died = 0;
	initialize_mutexes(&shared);
	initialize_forks(number_of_philosophers, philosophers, forks);
	initialize_philosophers(argc, argv, number_of_philosophers, philosophers,forks, &shared);
	create_threads(philosophers, thread, number_of_philosophers);
	end_threads(number_of_philosophers, thread);
	pthread_create(&monitor, NULL, &monitor_function, philosophers);
	pthread_join(monitor, NULL);
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
