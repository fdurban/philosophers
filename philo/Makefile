NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror -lpthread
CFILES = main.c philo_utils.c monitor_thread.c philo_thread.c initialize.c parse.c init_start.c
OBJS = $(CFILES:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all