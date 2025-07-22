NAME = philo
CC = gcc
CFLAGS = -fsanitize=thread  -Wall -Wextra -Werror -lpthread
CFILES = main.c philo_utils.c
all:
	$(CC) $(CFLAGS) $(CFILES) -o $(NAME)