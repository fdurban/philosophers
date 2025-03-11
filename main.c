/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/11 17:16:18 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include "philo.h"

void	*thread_function(void *arg)
{
	philo_t *philosophers = (philo_t *)arg;

	pthread_mutex_lock(philosophers->left_fork);
	pthread_mutex_lock(philosophers->right_fork);
	printf("I am the %d philo\n", philosophers->id);
	// I AM EATING
	//USLEEP(time to eat);
	pthread_mutex_unlock(philosophers->left_fork);
	pthread_mutex_unlock(philosophers->right_fork);
	//USLEEP(time to sleep)
	return (NULL);
}

int main()
{
	philo_t			philosophers[4];
	pthread_t		thread[4];
	pthread_mutex_t	write_lock1;
	int i;

	pthread_mutex_init(&write_lock1, NULL);
	for (int i = 0; i < 4; i++)
	{
		philosophers[i].id = i;
		philosophers[i].write_lock = &write_lock1;
	}
	i = 0;
	while (i < 4)
	{
		if (pthread_create(&thread[i], NULL, &thread_function, &philosophers[i]) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < 4)
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