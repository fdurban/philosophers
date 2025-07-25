/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernando <fernando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:49:41 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/24 02:54:08 by fernando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

# define RED "\033[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\033[0;33m"
# define BLUE "\033[0;34m"
# define CYAN "\033[0;96m"
# define RESET "\033[0m"

typedef struct s_shared_data
{
	pthread_mutex_t	dead;
	pthread_mutex_t	write;
	pthread_mutex_t	start_mutex;
	int				someone_died;
	int				number_of_philosophers;
	int				meals_required;
	long			start_time;
	int				start_flag;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
}	t_shared_data;

typedef struct s_philo
{
	int					id;
	long				time_of_last_meal;
	long				meals_eaten;

	pthread_mutex_t		*left_fork;
	pthread_mutex_t		*right_fork;
	pthread_mutex_t		meal_mutex;

	t_shared_data		*shared_data;
}	t_philo;

typedef struct s_philo_monitor_args
{
	t_philo	*philosophers;
	long	number_of_philosophers;
}	t_monitor_args;

typedef struct s_simulation
{
	int					argc;
	char				**argv;
	t_shared_data		shared;
	t_philo				*philosophers;
	pthread_t			*threads;
	pthread_t			monitor;
	pthread_mutex_t		*forks;
}	t_simulation;

int		parse_args(int argc, char **argv, t_simulation *sim);
int		validate_args(int argc);
long	ft_atol(const char *str);
int		has_anyone_died(t_philo *philo);
long	get_time_stamp(void);
void	clean_up(t_simulation	*simulation);
void	initialize_philosophers(t_simulation *simulation);
int		initialize_forks(t_simulation *sim);
void	initialize_mutexes(t_shared_data *shared);
void	*thread_function(void *arg);
void	print_message(t_philo *philo, long time, char *msg);
void	usleep_precise(long time, t_philo *philo);
void	*monitor_function(void *arg);
int		create_threads(t_philo *philosophers, pthread_t *thread,
			long number_of_philosophers);
int		initialize_simulation(int argc, char **argv, t_simulation *sim);
void	start_simulation(t_simulation *sim);
void	create_monitor_thread(t_simulation *sim);

#endif
