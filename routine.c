/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 17:43:21 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/15 17:39:36 by jfoeller         ###   ########.fr       */
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
	if (coder->dgl_adj[0]->dongle_id < coder->dgl_adj[1]->dongle_id)
	{
		if (pthread_mutex_lock(&coder->dgl_adj[0]->is_available) == 0)
			print_dongle(coder, coder->dgl_adj[0]->dongle_id);
		if (pthread_mutex_lock(&coder->dgl_adj[1]->is_available) == 0)
			print_dongle(coder, coder->dgl_adj[1]->dongle_id);
	}
	else
	{
		if (pthread_mutex_lock(&coder->dgl_adj[1]->is_available) == 0)
			print_dongle(coder, coder->dgl_adj[1]->dongle_id);
		if (pthread_mutex_lock(&coder->dgl_adj[0]->is_available) == 0)
			print_dongle(coder, coder->dgl_adj[0]->dongle_id);
	}
}

/**
 * @brief Simulates the compiling action of a coder and releases dongles.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_lock);
	coder->last_compile = current_time(coder);
	pthread_mutex_unlock(&coder->state_lock);
	print_action(coder, "is compiling");
	usleep(coder->config->time_compile * 1000);
	pthread_mutex_lock(&coder->state_lock);
	coder->count_compile++;
	pthread_mutex_unlock(&coder->state_lock);
	pthread_mutex_lock(&coder->sim->secure_heap);
	coder->dgl_adj[0]->in_use = false;
	coder->dgl_adj[1]->in_use = false;
	coder->dgl_adj[0]->free_time = current_time(coder);
	coder->dgl_adj[1]->free_time = current_time(coder);
	pthread_cond_broadcast(&coder->sim->wait_heap);
	pthread_mutex_unlock(&coder->sim->secure_heap);
	pthread_mutex_unlock(&coder->dgl_adj[0]->is_available);
	pthread_mutex_unlock(&coder->dgl_adj[1]->is_available);
}

/**
 * @brief Simulates the debugging action of a coder.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	debug(t_coder *coder)
{
	print_action(coder, "is debugging");
	usleep(coder->config->time_debug * 1000);
}

/**
 * @brief Simulates the refactoring action of a coder.
 * 
 * @param coder Pointer to the coder executing the action.
 */
void	refactor(t_coder *coder)
{
	print_action(coder, "is refactoring");
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
	if (coder->config->nb_coders == 1)
	{
		print_dongle(coder, coder->dgl_adj[0]->dongle_id);
		while (!check_stop(coder->sim))
			usleep(1000);
		return (NULL);
	}
	if (coder->coder_id % 2 == 0)
		usleep(1000);
	while (!check_stop(coder->sim))
	{
		if (!wait_for_turn(coder))
			break ;
		wait_both_cooldowns(coder);
		if (check_stop(coder->sim))
			break ;
		take_dongles(coder);
		compile(coder);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}
