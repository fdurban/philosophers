/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/13 18:56:18 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*thread_function(void *arg)
{
	philo_t *philo = (philo_t *)arg;
	
	while (1)
	{		
		if (philo->id % 2 == 0)
		{
			usleep(philo->time_to_sleep);
			printf("Philosopher %d is sleeping\n", philo->id);
		}
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->right_fork);
		printf(YELLOW "Philosopher %d takes left fork\n" RESET, philo->id);
		printf(YELLOW "Philosopher %d takes right fork\n" RESET, philo->id);
		printf(YELLOW "Philosopher %d is eating\n" RESET, philo->id);
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		usleep(philo->time_to_eat);
	}
		return (NULL);
}

int main(int argc, char **argv)
{
	printf("Number of arguments is %d\n", argc);
	philo_t			*philosophers;
	pthread_t		*thread;
	pthread_mutex_t	*forks;
	int				i;
	long			number_of_philosophers;
	
	philosophers = malloc(sizeof(philo_t) * ft_atol(argv[1]));
	forks = malloc(sizeof(pthread_mutex_t) * ft_atol(argv[1]));
	number_of_philosophers = ft_atol(argv[1]);
	thread = malloc(sizeof(pthread_t) * number_of_philosophers);
	pthread_mutex_init(&philosophers->write, NULL);
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].id = i;
		if (pthread_mutex_init(&forks[i], NULL) != 0)
		{
			perror("Error inicializando el mutex");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].time_to_die = ft_atol(argv[2]);
		philosophers[i].time_to_eat = ft_atol(argv[3]);
		philosophers[i].time_to_sleep = ft_atol(argv[4]);
		philosophers[i].left_fork = &forks[i];
		philosophers[i].right_fork = &forks[(i + 1) %  number_of_philosophers];
		i++;
	}
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
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_join(thread[i], NULL) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	printf("Terminan aquí\n");
	free(philosophers);
	free(thread);
	free(forks);
}
