/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/23 17:37:46 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	usleep_precise(long time, t_philo *philo)
{
	long	start;
	long	now;
	long	elapsed;
	long	remaining;

	start = get_time_stamp();
	while (1)
	{
		now = get_time_stamp();
		elapsed = now - start;
		if (elapsed >= time || has_anyone_died(philo))
			break ;
		remaining = time - elapsed;
		if (remaining > 1000)
			usleep(500);
		else if (remaining > 100)
			usleep(100);
		else
			usleep(10);
	}
}

int	create_threads(t_philo *philosophers,
	pthread_t *thread, long number_of_philosophers)
{
	int	i;

	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_create(&thread[i], NULL,
				&thread_function, &philosophers[i]) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	return (0);
}

int	end_threads(long number_of_philosophers, pthread_t *thread)
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

void	create_monitor_thread(t_simulation *sim)
{
	t_monitor_args	*args;

	args = malloc(sizeof(t_monitor_args));
	args->philosophers = sim->philosophers;
	args->number_of_philosophers = sim->number_of_philosophers;
	if (pthread_create(&sim->monitor, NULL, monitor_function, args) != 0)
	{
		perror("Error creando el monitor");
		clean_up(sim);
		exit(1);
	}
}

int	main(int argc, char **argv)
{
	t_simulation	simulation;

	if (!validate_args(argc))
		return (1);
	if (!initialize_simulation(argc, argv, &simulation))
		return (1);
	start_simulation(&simulation);
	pthread_join(simulation.monitor, NULL);
	end_threads(simulation.number_of_philosophers, simulation.threads);
	clean_up(&simulation);
	return (0);
}
