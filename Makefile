NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror
CFILES = main.c philo_utils.c

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(NAME)