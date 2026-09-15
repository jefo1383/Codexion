/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 14:12:03 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/15 16:11:46 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Frees all allocated memory and destroys mutexes.
 * 
 * @param sim Pointer to the main simulation structure.
 * @param heap Pointer to the priority queue structure.
 */
void	free_all(t_sim *sim, t_heap *heap)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].is_available);
		pthread_mutex_destroy(&sim->coders[i].state_lock);
		i++;
	}
	pthread_mutex_destroy(&sim->can_display);
	pthread_mutex_destroy(&sim->can_stop);
	pthread_mutex_destroy(&sim->secure_heap);
	pthread_cond_destroy(&sim->wait_heap);
	free(sim->dongles);
	free(sim->coders);
	free(heap->requests);
}

/**
 * @brief Calculates the priority value for
 * a coder's request based on the scheduler.
 * 
 * @param coder Pointer to the coder making the request.
 * @return size_t The calculated priority value.
 */
size_t	init_request(t_coder *coder)
{
	if (coder->config->scheduler == FIFO)
		return (current_time(coder));
	else
		return (coder->last_compile + coder->config->time_burnout);
}

/**
 * @brief Destroys initialized dongle mutexes and
 * condition variables, then frees the array.
 * 
 * @param sim Pointer to the main simulation structure.
 * @param count The number of successfully initialized dongles
 * (index before failure).
 */
void	rollback_dongles(t_sim *sim, int count)
{
	while (count >= 0)
	{
		pthread_mutex_destroy(&sim->dongles[count].is_available);
		count--;
	}
	free(sim->dongles);
}

/**
 * @brief Registers the coder in the heap and waits for its turn.
 *
 * @param coder Pointer to the coder requesting dongles.
 * @return bool true if it's the coder's turn, false if the simulation stopped.
 */
bool	wait_for_turn(t_coder *coder)
{
	if (pthread_mutex_lock(&coder->sim->secure_heap) == 0)
	{
		insert_request(&coder->sim->heap, coder, init_request(coder));
		bubble_up(&coder->sim->heap, coder->sim->heap.nb_requests - 1);
		while (coder->sim->heap.requests[0].coder != coder)
		{
			if (check_stop(coder->sim))
			{
				pthread_mutex_unlock(&coder->sim->secure_heap);
				return (false);
			}
			pthread_cond_wait(&coder->sim->wait_heap,
				&coder->sim->secure_heap);
		}
		extract_min(&coder->sim->heap);
		pthread_mutex_unlock(&coder->sim->secure_heap);
		pthread_cond_broadcast(&coder->sim->wait_heap);
	}
	return (true);
}

/**
 * @brief Pauses the coder thread until both adjacent dongles 
 *        have finished their cooldown period.
 *
 * @param coder Pointer to the coder attempting to compile.
 */
void	wait_both_cooldowns(t_coder *coder)
{
	size_t	dongles_cd;
	size_t	time;
	size_t	t1;
	size_t	t2;

	pthread_mutex_lock(&coder->dgl_adj[0]->is_available);
	t1 = coder->dgl_adj[0]->free_time;
	pthread_mutex_unlock(&coder->dgl_adj[0]->is_available);
	pthread_mutex_lock(&coder->dgl_adj[1]->is_available);
	t2 = coder->dgl_adj[1]->free_time;
	pthread_mutex_unlock(&coder->dgl_adj[1]->is_available);
	if (t1 > t2)
		dongles_cd = t1 + coder->config->cooldown;
	else
		dongles_cd = t2 + coder->config->cooldown;
	time = current_time(coder);
	if (time < dongles_cd)
		usleep((dongles_cd - time) * 1000);
}
