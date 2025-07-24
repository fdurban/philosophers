/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernando <fernando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 16:45:58 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/24 02:53:37 by fernando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	initialize_mutexes(t_shared_data *shared)
{
	pthread_mutex_init(&shared->dead, NULL);
	pthread_mutex_init(&shared->write, NULL);
	pthread_mutex_init(&shared->start_mutex, NULL);
	shared->someone_died = 0;
}

int	initialize_forks(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->shared.number_of_philosophers)
	{
		sim->philosophers[i].id = i + 1;
		if (pthread_mutex_init(&sim->forks[i], NULL) != 0)
		{
			perror("Error inicializando el mutex");
			return (1);
		}
		i++;
	}
	return (0);
}

void	initialize_philosophers(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->shared.number_of_philosophers)
	{
		simulation->philosophers[i].shared_data = &simulation->shared;
		simulation->philosophers[i].meals_eaten = 0;
		simulation->philosophers[i].time_of_last_meal = get_time_stamp();
		simulation->philosophers[i].left_fork = &simulation->forks[i];
		simulation->philosophers[i].right_fork
			= &simulation->forks[(i + 1) % simulation->shared.number_of_philosophers];
		pthread_mutex_init(&simulation->philosophers[i].meal_mutex, NULL);
		i++;
	}
}
