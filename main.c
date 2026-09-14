/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 15:21:42 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/14 15:53:14 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Manages the monitor thread and waits for all coder threads to finish.
 * 
 * @param sim Pointer to the main simulation structure.
 */
void	manage_threads(t_sim *sim)
{
	pthread_t	monitor;
	int			i;

	i = 0;
	pthread_create(&monitor, NULL, monitor_routine, sim);
	while (i < sim->config.nb_coders)
		pthread_join(sim->coders[i].thread_id, NULL);
	pthread_join(monitor, NULL);
}

/**
 * @brief Main entry point of the Codexion simulation.
 * 
 * @param argc Argument count.
 * @param argv Argument values.
 * @return int 0 on success, 1 on error.
 */
int	main(int argc, char **argv)
{
	t_sim	sim;

	if (!check_args(argc, argv))
		return (1);
	init_config(&sim.config, argv);
	if (!init_sim(&sim))
		return (1);
	manage_threads(&sim);
	free_all(&sim, &sim.heap);
	return (0);
}
