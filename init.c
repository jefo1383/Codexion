/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 13:57:47 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/14 15:19:26 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Initializes the dongles array and their mutexes
 * and allocates resources.
 * 
 * @param sim The main simulation structure.
 * @return true on success, false if a mutex init fails.
 */
bool	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	sim->dongles = malloc(sizeof(t_dongle) * sim->config.nb_coders);
	if (!sim->dongles)
		return (false);
	while (i < sim->config.nb_coders)
	{
		sim->dongles[i].dongle_id = i + 1;
		sim->dongles[i].free_time = 0;
		sim->dongles[i].in_use = false;
		if (pthread_mutex_init(&sim->dongles[i].is_available, NULL) != 0)
		{
			rollback_dongles(sim, i - 1);
			return (false);
		}
		if (pthread_cond_init(&sim->dongles[i].cond_wait, NULL) != 0)
		{
			rollback_dongles(sim, i - 1);
			pthread_mutex_destroy(&sim->dongles[i].is_available);
			return (false);
		}
		i++;
	}
	return (true);
}

/**
 * @brief Allocates and initializes the coders array, 
 * threads, and their variables.
 * 
 * @param sim The main simulation structure.
 * @return true on success, false on error.
 */
bool	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	sim->coders = malloc(sizeof(t_coder) * sim->config.nb_coders);
	if (!sim->coders)
		return (false);
	while (i < sim->config.nb_coders)
	{
		sim->coders[i].coder_id = i + 1;
		sim->coders[i].config = &sim->config;
		sim->coders[i].count_compile = 0;
		sim->coders[i].last_compile = 0;
		sim->coders[i].dgl_adj[0] = &sim->dongles[i];
		sim->coders[i].dgl_adj[1] = &sim->dongles[(
				(i - 1 + sim->config.nb_coders) % sim->config.nb_coders)];
		sim->coders[i].sim = sim;
		if (pthread_create(&sim->coders[i].thread_id,
				NULL, coder_routine, &sim->coders[i]) != 0)
		{
			free(sim->coders);
			return (false);
		}
		i++;
	}
	return (true);
}

/**
 * @brief Initializes all the global mutexes for the simulation.
 * 
 * @param sim Pointer to the main simulation structure.
 * @return bool true if successful, false if any mutex init fails.
 */
bool	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->can_display, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&sim->can_stop, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->can_display);
		return (false);
	}
	if (pthread_mutex_init(&sim->secure_heap, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->can_display);
		pthread_mutex_destroy(&sim->can_stop);
		return (false);
	}
	if (pthread_cond_init(&sim->wait_heap, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->can_display);
		pthread_mutex_destroy(&sim->can_stop);
		pthread_mutex_destroy(&sim->secure_heap);
		return (false);
	}
	return (true);
}

/**
 * @brief Initializes the main simulation structure.
 * 
 * @param sim Pointer to the simulation structure.
 * @return true if init is successful, false if memory allocation fails.
 */
bool	init_sim(t_sim *sim)
{
	sim->start_time = get_time_ms();
	if (!init_heap(&sim->heap, sim->config.nb_coders))
		return (false);
	if (!init_dongles(sim))
	{
		free(sim->heap.requests);
		return (false);
	}
	if (!init_coders(sim))
	{
		free(sim->heap.requests);
		free(sim->dongles);
		return (false);
	}
	sim->stop = false;
	if (!init_mutexes(sim))
	{
		free(sim->heap.requests);
		free(sim->dongles);
		free(sim->coders);
		return (false);
	}
	return (true);
}

/**
 * @brief Initializes the priority queue (min-heap).
 * 
 * @param heap Pointer to the heap structure to initialize.
 * @param max_capacity The maximum number of requests the heap can hold.
 * @return true if initialization is successful, false otherwise.
 */
bool	init_heap(t_heap *heap, int max_capacity)
{
	heap->requests = malloc(sizeof(t_request) * max_capacity);
	if (!heap->requests)
		return (false);
	heap->nb_requests = 0;
	heap->max_requests = max_capacity;
	return (true);
}
