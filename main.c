/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/11 16:45:28 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include "philo.h"

void	*thread_function(void *arg)
{
	philo_t *philosophers = (philo_t *)arg;
	pthread_mutex_lock(philosophers->write_lock);
	printf("I AM EATING ");
	printf("I am the %d philo\n", philosophers->id);
	pthread_mutex_unlock(philosophers->write_lock);
	return (NULL);
}

int main()
{
	philo_t			philosophers[5];
	pthread_t		thread[5];
	pthread_mutex_t	write_lock1[1];
	int i;

	pthread_mutex_init(&write_lock1[0], NULL);
	for (int i = 0; i < 5; i++)
	{
		philosophers[i].id = i;
		philosophers[i].write_lock = &write_lock1[0];
	}
	i = 0;
	while (i < 5)
	{
		if (pthread_create(&thread[i], NULL, &thread_function, &philosophers[i]) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < 5)
	{
		if (pthread_join(thread[i], NULL) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	printf("Terminan aquí\n");
}
	//philo_t		philosophers[5];
	// philo_t philosophers;
	// philosohers = (philo_t)name;
	// pthread_mutex_lock(left_fork);
	// pthread_mutex_lock(right_fork);
	// pthread_mutex_unlock(left_fork);
	// pthread_mutex_unlock(right_fork);