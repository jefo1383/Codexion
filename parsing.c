/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 13:48:22 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/03 10:32:49 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Checks if a string contains only digit characters.
 * 
 * @param str The string to check.
 * @return true if the string is a valid positive number, false otherwise.
 */
bool	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '0' || str[0] == '\0')
		return (false);
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Checks if the command line arguments are valid.
 * 
 * @param argc The argument count.
 * @param argv The argument values.
 * @return true if valid, false otherwise.
 */
bool	check_args(int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 9)
	{
		fprintf(stderr, "Error: Expected 8 arguments, got %d\n", argc - 1);
		return (false);
	}
	while (i < 8)
	{
		if (!is_valid_number(argv[i]))
		{
			fprintf(stderr, "Error: %s must be a positive number\n", argv[i]);
			return (false);
		}
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		fprintf(stderr, "%s must be 'fifo' or 'edf'\n", argv[8]);
		return (false);
	}
	return (true);
}

/**
 * @brief Initializes the configuration from valid arguments.
 * 
 * @param config Pointer to the config structure to fill.
 * @param argv The argument values.
 */
void	init_config(t_config *config, char **argv)
{
	
}