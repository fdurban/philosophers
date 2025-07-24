/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernando <fernando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 16:11:07 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/24 02:45:35 by fernando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_message(t_philo *philo, long time, char *msg)
{
	pthread_mutex_lock(&philo->shared_data->write);
	if (!has_anyone_died(philo))
		printf("%ld %d %s", time, philo->id, msg);
	pthread_mutex_unlock(&philo->shared_data->write);
}

long	get_time_stamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	has_anyone_died(t_philo *philo)
{
	int	dead;

	pthread_mutex_lock(&philo->shared_data->dead);
	dead = philo->shared_data->someone_died;
	pthread_mutex_unlock(&philo->shared_data->dead);
	return (dead);
}

long	ft_atol(const char *str)
{
	long	result;
	int		sign;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str++ == '-')
			sign = -1;
		else
			sign = 1;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}

void	clean_up(t_simulation	*simulation)
{
	int	i;

	i = 0;
	while (i < simulation->shared.number_of_philosophers)
	{
		pthread_mutex_destroy(&simulation->forks[i]);
		pthread_mutex_destroy(&simulation->philosophers[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&simulation->shared.dead);
	pthread_mutex_destroy(&simulation->shared.write);
	pthread_mutex_destroy(&simulation->shared.start_mutex);
	free(simulation->philosophers);
	free(simulation->threads);
	free(simulation->forks);
}
