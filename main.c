/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/04/08 14:03:29 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time_stamp(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int has_anyone_died(philo_t *philo)
{

	pthread_mutex_lock(philo->check_dead);
	if (*philo->someone_died)
	{
		pthread_mutex_unlock(philo->check_dead);
		return (1);
	}
	if (get_time_stamp() - philo->time_of_last_meal > philo->time_to_die)
	{
		printf("%ld %d died\n", get_time_stamp() - philo->time_of_last_meal, philo->id);
		*philo->someone_died = 1;
		pthread_mutex_unlock(philo->check_dead);
		return (1);
	}
	pthread_mutex_unlock(philo->check_dead);
	return (0);
}
static void	print_message(philo_t *philo, long time, char *msg)
{
	pthread_mutex_lock(philo->write);
	// printf("Confirmacion de que le filosofo ha muerto %d\n", has_anyone_died(philo));
	if (has_anyone_died(philo))
	{
		pthread_mutex_unlock(philo->write);
		return ;
	}
	printf("%ld %d %s", time, philo->id, msg);
	pthread_mutex_unlock(philo->write);
}

	// FUNCION QUE HACE USO DE USLEEP PERO CON COMPROBACION DE MUERTE CADA VEZ (COMPROBAR HAS_ANYONE_DIED)
void	usleep_precise(long time, philo_t *philo)
{
	long	start;
	
	start = get_time_stamp();
	while(get_time_stamp() - start < time)
	{
		//printf("Confirmacion de que le filosofo ha muerto %d\n", has_anyone_died(philo));
		if(has_anyone_died(philo))
		{
			break ;
		}
		usleep(100);
	}
	return ;
}

void	*thread_function(void *arg)
{
	philo_t *philo = (philo_t *)arg;
	long start_time = get_time_stamp();

	pthread_mutex_lock(philo->meal_mutex);
	philo->time_of_last_meal = get_time_stamp();
	pthread_mutex_unlock(philo->meal_mutex);
	if (philo->id % 2 != 0)
	{
/* 		print_message(philo, get_time_stamp() - timestamp, "is sleeping\n"); */
		usleep_precise(philo->time_to_sleep, philo);
	}
	while (!*philo->someone_died)
	{
		//COGEN LOS TENEDORES
		pthread_mutex_lock(philo->left_fork);
		print_message(philo, get_time_stamp() - start_time, "has taken a fork\n");
		pthread_mutex_lock(philo->right_fork);
		print_message(philo, get_time_stamp() - start_time, "has taken a fork\n");
		//LOS FILOSOFOS EMPIEZAN A COMER
		pthread_mutex_lock(philo->meal_mutex);
		philo->time_of_last_meal = get_time_stamp();
		print_message(philo, get_time_stamp() - start_time, "is eating\n");	
		pthread_mutex_unlock(philo->meal_mutex);
		usleep_precise(philo->time_to_eat, philo);
		philo->meals_eaten++;
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
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

int	initialize_forks(long	number_of_philosophers, philo_t	philosophers[PHILO_MAX], pthread_mutex_t forks[PHILO_MAX])
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

int	initialize_philosophers(int argc, char **argv, long number_of_philosophers,
 philo_t philosophers[PHILO_MAX], pthread_mutex_t forks[PHILO_MAX], pthread_mutex_t dead, pthread_mutex_t check_dead,
 pthread_mutex_t meal_mutex, int someone_died, pthread_mutex_t write)
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
		philosophers[i].dead = &dead;
		philosophers[i].check_dead = &check_dead;
		philosophers[i].meal_mutex = &meal_mutex;
		philosophers[i].someone_died = &someone_died;
		philosophers[i].write = &write;
		i++;
	}
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

int main(int argc, char **argv)
{
	philo_t			philosophers[PHILO_MAX];
	pthread_t		thread[PHILO_MAX];
	pthread_mutex_t	forks[PHILO_MAX];
	pthread_mutex_t dead;
	pthread_mutex_t check_dead;
	pthread_mutex_t meal_mutex;
	pthread_mutex_t write;
	int				i;
	long			number_of_philosophers;
	int				someone_died;

	number_of_philosophers = ft_atol(argv[1]);
	someone_died = 0;
	pthread_mutex_init(&dead, NULL);
	pthread_mutex_init(&check_dead, NULL);
	pthread_mutex_init(&write, NULL);
	pthread_mutex_init(&meal_mutex, NULL);
	initialize_forks(number_of_philosophers, philosophers, forks);
	initialize_philosophers(argc, argv, number_of_philosophers, philosophers,forks, dead, check_dead, meal_mutex, someone_died, write);
	create_threads(philosophers, thread, number_of_philosophers);
	end_threads(number_of_philosophers, thread);
	pthread_mutex_destroy(&dead);
	pthread_mutex_destroy(&check_dead);
	pthread_mutex_destroy(&meal_mutex);
	pthread_mutex_destroy(&write);
	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_mutex_destroy(&forks[i]);
		i++;
	}
	printf("Terminan aquí\n");
}
