#ifndef PHILO_H
#define PHILO_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "philo.h"

typedef struct philo {

	int id;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	pthread_mutex_t	*write_lock;

} philo_t;

long	ft_atol(const char *str);

#endif