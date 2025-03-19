/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdurban- <fdurban-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:35:19 by fdurban-          #+#    #+#             */
/*   Updated: 2025/03/19 18:06:09 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time_stamp()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	*thread_function(void *arg)
{
	philo_t *philo = (philo_t *)arg;
	long timestamp = get_time_stamp();

	if (philo->id % 2 != 0)
		usleep(philo->time_to_sleep * 1000);
	while (1)
	{
		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if(*philo->someone_died)
		{
			pthread_mutex_unlock(philo->dead);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE

		//EMPIEZA A PENSAR
		printf(CYAN "%ld %d is thinking\n" RESET, get_time_stamp() - timestamp, philo->id);
		if(philo->time_to_think > 0)
			usleep(philo->time_to_think * 1000);
		//TERMINA DE PENSAR

		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if(*philo->someone_died)
		{
			pthread_mutex_unlock(philo->dead);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE

		//EMPIEZA COMER
		pthread_mutex_lock(philo->right_fork);
		printf(YELLOW "%ld %d has taken a fork\n" RESET, get_time_stamp() - timestamp, philo->id);		
		pthread_mutex_lock(philo->left_fork);
		printf(YELLOW "%ld %d has taken a fork\n" RESET, get_time_stamp() - timestamp, philo->id);
		printf(GREEN "%ld %d is eating\n" RESET, get_time_stamp() - timestamp, philo->id);
		pthread_mutex_lock(philo->meal_mutex);
		philo->time_of_last_meal = get_time_stamp();
		pthread_mutex_unlock(philo->meal_mutex);
		usleep(philo->time_to_eat * 1000);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		//TERMINA COMER

		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if (get_time_stamp() - philo->time_of_last_meal > philo->time_to_die)
		{
			printf(RED "%ld %d died\n" RESET, get_time_stamp() - timestamp, philo->id);
			*philo->someone_died = 1;
			pthread_mutex_unlock(philo->dead);
			pthread_mutex_unlock(philo->left_fork);
			pthread_mutex_unlock(philo->right_fork);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE

		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if(*philo->someone_died)
		{
			pthread_mutex_unlock(philo->dead);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE
		
		//EMPIEZA DORMIR
		printf(BLUE "%ld %d is sleeping\n" RESET, get_time_stamp() - timestamp, philo->id);
		usleep(philo->time_to_sleep * 1000);
		//TERMINA DORMIR
		
		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if(*philo->someone_died)
		{
			pthread_mutex_unlock(philo->dead);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE
		
		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if (get_time_stamp() - philo->time_of_last_meal > philo->time_to_die)
		{
			printf(RED "%ld %d died\n" RESET, get_time_stamp() - timestamp, philo->id);
			*philo->someone_died = 1;
			pthread_mutex_unlock(philo->dead);
			pthread_mutex_unlock(philo->left_fork);
			pthread_mutex_unlock(philo->right_fork);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE

		//INICIO COMPRUEBA MUERTE
		pthread_mutex_lock(philo->dead);
		if(*philo->someone_died)
		{
			pthread_mutex_unlock(philo->dead);
			return (NULL);
		}
		pthread_mutex_unlock(philo->dead);
		// FIN COMPRUEBA MUERTE
	}
	return (NULL);
}

int main(int argc, char **argv)
{
	printf("Number of arguments is %d\n", argc);
	philo_t			*philosophers;
	pthread_t		*thread;
	pthread_mutex_t	*forks;
	pthread_mutex_t *dead;
	pthread_mutex_t *meal_mutex;
	int				i;
	long			number_of_philosophers;
	int				someone_died;
	
	philosophers = malloc(sizeof(philo_t) * ft_atol(argv[1]));
	forks = malloc(sizeof(pthread_mutex_t) * ft_atol(argv[1])); // 5
	dead = malloc(sizeof(pthread_mutex_t)); // 1
	meal_mutex = malloc(sizeof(pthread_mutex_t)); // 1
	number_of_philosophers = ft_atol(argv[1]);
	thread = malloc(sizeof(pthread_t) * number_of_philosophers);
	someone_died = 0;
	//pthread_mutex_init(&philosophers->write, NULL);
	pthread_mutex_init(dead, NULL);
	pthread_mutex_init(meal_mutex, NULL);
	write(1,"Entra\n", 6);
	i = 0;
	while (i < number_of_philosophers)
	{
		philosophers[i].id = i;
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
		philosophers[i].time_to_think = philosophers[i].time_to_die - (philosophers[i].time_to_sleep + philosophers[i].time_to_eat);
		philosophers[i].left_fork = &forks[i];
		philosophers[i].right_fork = &forks[(i + 1) %  number_of_philosophers];
		philosophers[i].dead = dead;
		philosophers[i].meal_mutex = meal_mutex;
		philosophers[i].someone_died = &someone_died;
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