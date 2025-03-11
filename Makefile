NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror
CFILES = main.c

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(NAME)