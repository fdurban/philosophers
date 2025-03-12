/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/12 19:01:45 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*thread_function(void *arg)
{
	philo_t *philosophers = (philo_t *)arg;

	if (philosophers->id % 2 == 0)
	{
		pthread_mutex_lock(&philosophers[philosophers->id].fork);
		printf("Philosopher %d is eating\n", philosophers->id);
		usleep(philosophers->time_to_eat);
	}
	else
	{
		printf("Philosopher %d is sleeping\n", philosophers->id);
		usleep(philosophers->time_to_sleep);
	}
	if (philosophers->id % 2 == 0)
	{
		pthread_mutex_unlock(&philosophers[philosophers->id].fork);
		usleep(philosophers->time_to_eat);
	}
	return (NULL);
}

int main(int argc, char **argv)
{
	printf("Number of arguments is %d\n", argc);
	philo_t			*philosophers;
	pthread_t		*thread;
	int				i;
	long			number_of_philosophers;
	
	philosophers = malloc(sizeof(philo_t) * ft_atol(argv[1]));
	number_of_philosophers = ft_atol(argv[1]);
	philosophers->time_to_die = ft_atol(argv[2]);
	philosophers->time_to_eat = ft_atol(argv[3]);
	philosophers->time_to_sleep = ft_atol(argv[4]);
	thread = malloc(sizeof(pthread_t) * number_of_philosophers);
	
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_mutex_init(&philosophers[i].fork, NULL) != 0)
		{
			perror("Error inicializando el mutex");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers->id = i;
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
}
