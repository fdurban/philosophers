NAME = philo
CC = gcc
CFLAGS = -g  -Wall -Wextra -Werror  -lpthread
CFILES = main.c philo_utils.c
all:
	$(CC) $(CFLAGS) $(CFILES) -o $(NAME)

# -fsanitize=thread