#ifndef PHILO_H
#define PHILO_H
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;96m"
#define RESET "\033[0m"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "philo.h"


typedef struct philo {

	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*dead;
	pthread_mutex_t	*meal_mutex;
	int				id;
	long			time_to_eat;
	long			time_of_last_meal;
	long			time_to_sleep;
	long			time_to_die;
	long			time_to_think;
	int				*someone_died;
} philo_t;

typedef struct personal_timeval
{
	time_t tv_sec;
	__suseconds_t tv_usec;
} personal_timeval_t ;

long	ft_atol(const char *str);

#endif