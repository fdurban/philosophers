#ifndef PHILO_H
#define PHILO_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "philo.h"

typedef struct philo {

	pthread_mutex_t	fork;
	int				id;
	long			time_to_eat;
	long			time_to_sleep;
	long			time_to_die;
} philo_t;

long	ft_atol(const char *str);

#endif