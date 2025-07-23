/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_start.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:43:55 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/23 17:41:10 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	initialize_simulation(int argc, char **argv, t_simulation *sim)
{
	sim->argc = argc;
	sim->argv = argv;
	if (!parse_args(argc, argv, sim))
		return (0);
	sim->number_of_philosophers = ft_atol(argv[1]);
	sim->shared.someone_died = 0;
	sim->shared.start_flag = 0;
	sim->philosophers = malloc(sizeof(t_philo) * sim->number_of_philosophers);
	sim->threads = malloc(sizeof(pthread_t) * sim->number_of_philosophers);
	sim->forks = malloc(sizeof(pthread_mutex_t) * sim->number_of_philosophers);
	if (!sim->philosophers || !sim->threads || !sim->forks)
		return (perror("Error al reservar memoria"), 0);
	sim->shared.meals_required = sim->number_of_times_must_eat;
	return (1);
}

void	start_simulation(t_simulation *sim)
{
	initialize_mutexes(&sim->shared);
	initialize_forks(sim);
	initialize_philosophers(sim);
	create_threads(sim->philosophers, sim->threads,
		sim->number_of_philosophers);
	create_monitor_thread(sim);
	pthread_mutex_lock(&sim->shared.start_mutex);
	sim->shared.start_time = get_time_stamp();
	sim->shared.start_flag = 1;
	pthread_mutex_unlock(&sim->shared.start_mutex);
}
