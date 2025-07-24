/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_thread.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernando <fernando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:25:41 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/24 02:44:45 by fernando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	wait_for_start(t_philo *philosophers)
{
	int	started;

	started = 0;
	while (!started)
	{
		pthread_mutex_lock(&philosophers->shared_data->start_mutex);
		started = philosophers->shared_data->start_flag;
		pthread_mutex_unlock(&philosophers->shared_data->start_mutex);
		if (!started)
			usleep(50);
	}
}

static int	check_if_philo_died(t_philo *philo)
{
	long	current_time;
	long	time_since_last_meal;

	pthread_mutex_lock(&philo->meal_mutex);
	current_time = get_time_stamp();
	time_since_last_meal = current_time - philo->time_of_last_meal;
	if (time_since_last_meal > philo->shared_data->time_to_die)
	{
		pthread_mutex_unlock(&philo->meal_mutex);
		pthread_mutex_lock(&philo->shared_data->dead);
		if (!philo->shared_data->someone_died)
		{
			philo->shared_data->someone_died = 1;
			printf("%ld %d died\n",
				get_time_stamp() - philo->shared_data->start_time,
				philo->id);
		}
		pthread_mutex_unlock(&philo->shared_data->dead);
		return (1);
	}
	pthread_mutex_unlock(&philo->meal_mutex);
	return (0);
}

void	*monitor_function(void *arg)
{
	t_monitor_args	*args;
	t_philo			*philosophers;
	long			num;
	int				i;

	args = (t_monitor_args *)arg;
	philosophers = args->philosophers;
	num = args->number_of_philosophers;
	wait_for_start(philosophers);
	while (1)
	{
		i = 0;
		while (i < num && philosophers[i].id != 0)
		{
			if (check_if_philo_died(&philosophers[i]))
			{
				free(arg);
				return (NULL);
			}
			i++;
		}
		usleep(100);
	}
	free(arg);
	return (NULL);
}
