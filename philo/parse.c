/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernando <fernando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:18:00 by fdurban-          #+#    #+#             */
/*   Updated: 2025/07/24 02:27:56 by fernando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_isdigit(int c)
{
	if ((c >= 48 && c <= 57))
	{
		return (1);
	}
	return (0);
}

int	is_str_numeric(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	if (str[0] == '+' || str[0] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	parse_args(int argc, char **argv, t_simulation *sim)
{
	sim->shared.number_of_philosophers = ft_atol(argv[1]);
	sim->shared.time_to_die = ft_atol(argv[2]);
	sim->shared.time_to_eat = ft_atol(argv[3]);
	sim->shared.time_to_sleep = ft_atol(argv[4]);
	if (argc == 6)
		sim->shared.meals_required = ft_atol(argv[5]);
	else
		sim->shared.meals_required = -1;
	if (sim->shared.number_of_philosophers <= 0 || sim->shared.time_to_die < 0
		|| sim->shared.time_to_eat < 0 || sim->shared.time_to_sleep < 0
		|| (argc == 6 && sim->shared.meals_required < 0)
		|| !is_str_numeric(argv[1]) || !is_str_numeric(argv[2])
		|| !is_str_numeric(argv[3]) || !is_str_numeric(argv[4])
		|| (argc == 6 && !is_str_numeric(argv[5])))
	{
		printf("Error: argumentos inválidos\n");
		return (0);
	}
	return (1);
}

int	validate_args(int argc)
{
	if (argc != 5 && argc != 6)
	{
		printf("Not the right number of arguments\n");
		return (0);
	}
	return (1);
}
