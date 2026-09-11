/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 14:12:03 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/11 17:02:06 by jfoeller         ###   ########.fr       */
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
		pthread_cond_destroy(&sim->dongles[i].cond_wait);
		i++;
	}
	pthread_mutex_destroy(&sim->can_display);
	pthread_mutex_destroy(&sim->can_stop);
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
		pthread_cond_destroy(&sim->dongles[count].cond_wait);
		pthread_mutex_destroy(&sim->dongles[count].is_available);
		count--;
	}
	free(sim->dongles);
}
