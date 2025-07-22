NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror -lpthread
CFILES = main.c philo_utils.c create_thread.c initialize.c
all:
	$(CC) $(CFLAGS) $(CFILES) -o $(NAME)