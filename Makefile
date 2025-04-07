NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror
CFILES = main.c philo_utils.c
# -fsanitize=thread 
all:
	$(CC) $(CFLAGS) $(CFILES) -o $(NAME)