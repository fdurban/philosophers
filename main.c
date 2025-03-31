/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/31 17:22:47 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time_stamp()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

static int is_dead(philo_t *philo)
{
	pthread_mutex_lock(philo->dead);
	if(*philo->someone_died)
	{
		pthread_mutex_unlock(philo->dead);
		return (1);
	}
	pthread_mutex_unlock(philo->dead);
	return (0);
}
static void	print_message(philo_t *philo, long time, char *msg)
{
	if (is_dead(philo))
		return ;
	pthread_mutex_lock(philo->write);
	printf("%ld %d %s", time, philo->id, msg);
	pthread_mutex_unlock(philo->write);
}

static int has_anyone_died(philo_t *philo, long timestamp)
{
	pthread_mutex_lock(philo->check_dead);
	if (get_time_stamp() - philo->time_of_last_meal > philo->time_to_die)
	{
		print_message(philo, get_time_stamp() - timestamp, "died\n");
		*philo->someone_died = 1;
		pthread_mutex_unlock(philo->check_dead);
		return (1);
	}
	pthread_mutex_unlock(philo->check_dead);
	return (0);
}

void	usleep_precise(long time)
{
	long	start;

	start = get_time_stamp();
	while(get_time_stamp() - start > time)
		usleep(time / 5);
}

void	*thread_function(void *arg)
{
	philo_t *philo = (philo_t *)arg;
	long timestamp = get_time_stamp();

	
	if (philo->id % 2 != 0)
		usleep(philo->time_to_sleep * 1000);
	while (1)
	{
		print_message(philo, get_time_stamp() - timestamp, "is thinking\n");
		if(philo->time_to_think > 0)
			usleep(philo->time_to_think * 1000);
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, get_time_stamp() - timestamp, "has taken a fork\n");
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, get_time_stamp() - timestamp, "has taken a fork\n");		
		}
		else
		{
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, get_time_stamp() - timestamp, "has taken a fork\n");
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, get_time_stamp() - timestamp, "has taken a fork\n");	
		}
		pthread_mutex_lock(philo->meal_mutex);
		philo->time_of_last_meal = get_time_stamp();
		print_message(philo, get_time_stamp() - timestamp, "is eating\n");	
		pthread_mutex_unlock(philo->meal_mutex);
		usleep(philo->time_to_eat * 1000);
		philo->meals_eaten++;
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		if (philo->number_of_times_each_philosopher_must_eat && (philo->number_of_times_each_philosopher_must_eat < philo->meals_eaten))
			break;
		if (has_anyone_died(philo, timestamp) || is_dead(philo))
			return (NULL);
		print_message(philo, get_time_stamp() - timestamp, "is sleeping\n");
		usleep(philo->time_to_sleep * 1000);
		if (has_anyone_died(philo, timestamp) || is_dead(philo))
			return (NULL);
	}
	return (NULL);
}

int main(int argc, char **argv)
{
	philo_t			*philosophers;
	pthread_t		*thread;
	pthread_mutex_t	*forks;
	pthread_mutex_t *dead;
	pthread_mutex_t *check_dead;
	pthread_mutex_t *meal_mutex;
	pthread_mutex_t *write;
	int				i;
	long			number_of_philosophers;
	int				someone_died;
	
	philosophers = malloc(sizeof(philo_t) * ft_atol(argv[1]));
	forks = malloc(sizeof(pthread_mutex_t) * ft_atol(argv[1]));
	dead = malloc(sizeof(pthread_mutex_t));
	check_dead = malloc(sizeof(pthread_mutex_t));
	meal_mutex = malloc(sizeof(pthread_mutex_t));
	number_of_philosophers = ft_atol(argv[1]);
	thread = malloc(sizeof(pthread_t) * number_of_philosophers);
	someone_died = 0;
	write = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(dead, NULL);
	pthread_mutex_init(check_dead, NULL);
	pthread_mutex_init(meal_mutex, NULL);
	pthread_mutex_init(write, NULL);
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].id = i + 1;
		if (pthread_mutex_init(&forks[i], NULL) != 0)
		{
			perror("Error inicializando el mutex");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].time_to_die = ft_atol(argv[2]);
		philosophers[i].time_to_eat = ft_atol(argv[3]);
		philosophers[i].time_to_sleep = ft_atol(argv[4]);
		if(argc == 6)
		{			
			philosophers[i].number_of_times_each_philosopher_must_eat = ft_atol(argv[5]);
			philosophers[i].meals_eaten = 0;
		}
		philosophers[i].time_to_think = philosophers[i].time_to_die - (philosophers[i].time_to_sleep + philosophers[i].time_to_eat);
		philosophers[i].left_fork = &forks[i];
		philosophers[i].right_fork = &forks[(i + 1) %  number_of_philosophers];
		philosophers[i].dead = dead;
		philosophers[i].check_dead = check_dead;
		philosophers[i].meal_mutex = meal_mutex;
		philosophers[i].someone_died = &someone_died;
		philosophers[i].write = write;
		i++;
	}
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_create(&thread[i], NULL, &thread_function, &philosophers[i]) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_join(thread[i], NULL) != 0)
		{
			perror("Error al crear el hilo");
			return (1);
		}
		i++;
	}
	printf("Terminan aquí\n");
	free(philosophers);
	free(thread);
	free(forks);
}

		// if(get_time_stamp() - (philo->time_to_eat + philo->time_to_sleep) > philo-> time_to_die)
		// {
		// 	printf(RED "[%ld ms] Philosopher %d died\n" RESET, get_time_stamp() - timestamp, philo->id);
		// 	return (NULL);
		// }