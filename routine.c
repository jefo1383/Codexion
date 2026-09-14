/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 17:43:21 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/14 13:27:04 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		printf("%zu %d has taken dongle %d\n",
			current_time(coder), coder->coder_id,
			coder->dgl_adj[0]->dongle_id);
		pthread_mutex_unlock(&coder->sim->can_display);
	}
	if (pthread_mutex_lock(&coder->dgl_adj[1]->is_available) == 0)
	{
		pthread_mutex_lock(&coder->sim->can_display);
		printf("%zu %d has taken dongle %d\n",
			current_time(coder), coder->coder_id,
			coder->dgl_adj[1]->dongle_id);
		pthread_mutex_unlock(&coder->sim->can_display);
	}
}

/**
 * @brief Simulates the compiling action of a coder.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	compile(t_coder *coder)
{
	coder->last_compile = current_time(coder);
	pthread_mutex_lock(&coder->sim->can_display);
	printf("%zu %d is compiling\n",
		current_time(coder), coder->coder_id);
	pthread_mutex_unlock(&coder->sim->can_display);
	usleep(coder->config->time_compile * 1000);
	coder->count_compile++;
	coder->dgl_adj[0]->free_time = current_time(coder);
	pthread_mutex_unlock(&coder->dgl_adj[0]->is_available);
	coder->dgl_adj[1]->free_time = current_time(coder);
	pthread_mutex_unlock(&coder->dgl_adj[1]->is_available);
	pthread_cond_broadcast(&coder->sim->wait_heap);
}

/**
 * @brief Simulates the debugging action of a coder.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	debug(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->can_display);
	printf("%zu %d is debugging\n",
		current_time(coder), coder->coder_id);
	pthread_mutex_unlock(&coder->sim->can_display);
	usleep(coder->config->time_debug * 1000);
}

/**
 * @brief Simulates the refactoring action of a coder.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	refactor(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->can_display);
	printf("%zu %d is refactoring\n",
		current_time(coder), coder->coder_id);
	pthread_mutex_unlock(&coder->sim->can_display);
	usleep(coder->config->time_refactor * 1000);
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
		wait_for_turn(coder);
		wait_both_cooldowns(coder);
		take_dongles(coder);
		compile(coder);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}
