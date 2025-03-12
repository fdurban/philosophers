/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/12 16:18:39 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*thread_function(void *arg)
{
	philo_t *philosophers = (philo_t *)arg;

	if(philosophers->id % 2 != 0)
	{
		pthread_mutex_lock(philosophers->write_lock);
		usleep(1000000);
		printf("Philosopher %d is sleeping\n", philosophers->id);
	}
	else
	{
		printf("Philosopher %d is eating\n", philosophers->id);
		
	}
	if(philosophers->id % 2 != 0)
	{
		pthread_mutex_unlock(philosophers->write_lock);
		usleep(1000000);
	}
	return (NULL);
}

int main(int argc, char **argv)
{
	philo_t			*philosophers;
	pthread_t		thread[100];
	pthread_mutex_t	write_lock1;
	int i;
	long philo_arg;
	
	printf("Number of arguments is %d\n", argc);
	philo_arg = ft_atol(argv[1]);
	philosophers = malloc(sizeof(philo_t) * philo_arg);
	pthread_mutex_init(&write_lock1, NULL);
	for (int i = 0; i < 100; i++)
	{
		philosophers[i].id = i;
		philosophers[i].write_lock = &write_lock1;
	}
	i = 0;
	while (i < 100)
	{
		if (pthread_create(&thread[i], NULL, &thread_function, &philosophers[i]) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < 100)
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