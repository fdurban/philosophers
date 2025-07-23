/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_thread.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 16:48:37 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/23 18:37:01 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	wait_for_start(t_philo *philo)
{
	int	started;

	started = 0;
	while (!started)
	{
		pthread_mutex_lock(&philo->shared_data->start_mutex);
		started = philo->shared_data->start_flag;
		pthread_mutex_unlock(&philo->shared_data->start_mutex);
		if (!started)
			usleep(50);
	}
}

static void	take_forks(t_philo *philo, long start_time)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		print_message(philo, get_time_stamp() - start_time,
			"has taken a fork\n");
		pthread_mutex_lock(philo->right_fork);
		print_message(philo, get_time_stamp() - start_time,
			"has taken a fork\n");
	}
	else
	{
		usleep(1000);
		pthread_mutex_lock(philo->right_fork);
		print_message(philo, get_time_stamp() - start_time,
			"has taken a fork\n");
		pthread_mutex_lock(philo->left_fork);
		print_message(philo, get_time_stamp() - start_time,
			"has taken a fork\n");
	}
}

static void	eat_sleep_think(t_philo *philo, long start_time)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->time_of_last_meal = get_time_stamp();
	print_message(philo, get_time_stamp() - start_time,
		"is eating\n");
	pthread_mutex_unlock(&philo->meal_mutex);
	usleep_precise(philo->time_to_eat, philo);
	philo->meals_eaten++;
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
	print_message(philo, get_time_stamp() - start_time,
		"is sleeping\n");
	usleep_precise(philo->time_to_sleep, philo);
	print_message(philo, get_time_stamp() - start_time,
		"is thinking\n");
}

void	*thread_function(void *arg)
{
	t_philo	*philo;
	long	start_time;

	philo = (t_philo *)arg;
	wait_for_start(philo);
	pthread_mutex_lock(&philo->meal_mutex);
	philo->time_of_last_meal = get_time_stamp();
	pthread_mutex_unlock(&philo->meal_mutex);
	start_time = philo->shared_data->start_time;
	if (philo->id % 2 != 0 && philo->shared_data->number_of_philosophers == 1)
		usleep_precise(philo->time_to_eat, philo);
	while (!has_anyone_died(philo))
	{
			printf("Num philo %d\n", philo->shared_data->number_of_philosophers);
		if (philo->shared_data->number_of_philosophers == 1)
		{
				pthread_mutex_lock(philo->left_fork);
				print_message(philo, get_time_stamp() - start_time,
				"has taken a fork\n");
				usleep(philo->time_to_die * 1000);
				pthread_mutex_unlock(philo->left_fork);
				return (NULL);
		}
		if (philo->shared_data->meals_required != -1
			&& philo->meals_eaten >= philo->shared_data->meals_required)
			break ;
		take_forks(philo, start_time);
		eat_sleep_think(philo, start_time);
	}
	return (NULL);
}
