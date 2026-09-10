/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 10:01:44 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/10 14:57:35 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Gets the time in milliseconds.
 * 
 * @return size_t The current time in milliseconds.
 */
size_t	get_time_ms(void)
{
	struct timeval	time_data;

	gettimeofday(&time_data, NULL);
	return (((size_t)time_data.tv_sec * 1000) + (
			(size_t)time_data.tv_usec / 1000));
}

/**
 * @brief Gets the current relative time in milliseconds.
 * 
 * @return size_t The current relative time in milliseconds.
 */
size_t	current_time(t_coder *coder)
{
	return (get_time_ms() - coder->sim->start_time);
}

/**
 * @brief Safely checks if the simulation should stop.
 * 
 * @param sim The main simulation structure.
 * @return true if the simulation is stopped, false otherwise.
 */
bool	check_stop(t_sim *sim)
{
	bool	check;

	pthread_mutex_lock(&sim->can_stop);
	check = sim->stop;
	pthread_mutex_unlock(&sim->can_stop);
	return (check);
}

/**
 * @brief Checks if any coder burned out or if all finished compiling.
 * 
 * @param sim Pointer to the simulation structure.
 * @return true if the simulation should stop, false otherwise.
 */
bool	check_conditions(t_sim *sim)
{
	int	finished_coders;
	int	i;

	finished_coders = 0;
	i = 0;
	while (i < sim->config.nb_coders)
	{
		if ((current_time(&sim->coders[i]) - sim->coders[i].last_compile)
			> sim->config.time_burnout)
		{
			pthread_mutex_lock(&sim->can_display);
			printf("%zu %d burned out\n",
				current_time(&sim->coders[i]), sim->coders[i].coder_id);
			pthread_mutex_unlock(&sim->can_display);
			return (true);
		}
		if (sim->coders[i].count_compile >= sim->config.nb_compiles)
			finished_coders++;
		i++;
	}
	if (finished_coders == sim->config.nb_coders)
		return (true);
	return (false);
}

/**
 * @brief Routine for the monitor thread checking for end conditions.
 * 
 * @param arg Pointer to the simulation structure.
 * @return void* Always NULL.
 */
void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = arg;
	while (!check_stop(sim))
	{
		if (check_conditions(sim))
		{
			pthread_mutex_lock(&sim->can_stop);
			sim->stop = true;
			pthread_mutex_unlock(&sim->can_stop);
		}
	}
	return (NULL);
}
