/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 17:43:21 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/04 11:57:31 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Gets the current time in milliseconds.
 * 
 * @return size_t The current time in milliseconds.
 */
size_t	get_current_time_ms(void)
{
	
}

/**
 * @brief Locks the left and right dongles for the coder and logs the actions.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	take_dongles(t_coder *coder)
{
	if (pthread_mutex_lock(&coder->dgl_adj[0]->is_available) == 0)
	{
		pthread_mutex_lock(&coder->sim->can_display);
		printf("%d %d has taken dongle %d", coder->coder_id, coder->dgl_adj[0]->dongle_id);
		pthread_mutex_unlock(&coder->sim->can_display);
	}
	if (pthread_mutex_lock(&coder->dgl_adj[1]->is_available) == 0)
	{
		pthread_mutex_lock(&coder->sim->can_display);
		printf("%d %d has taken dongle %d", coder->coder_id, coder->dgl_adj[1]->dongle_id);
		pthread_mutex_unlock(&coder->sim->can_display);
	}
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
 * @brief The main routine executed by each coder thread.
 * 
 * @param arg Pointer to the coder's data.
 * @return void* Always NULL.
 */
void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = arg;
	if (coder->coder_id % 2 == 0)
		usleep(1);
	while (!check_stop(coder->sim))
	{
		
	}
}
