/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 14:16:09 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/03 11:05:19 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <time.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>

/**
 * @brief Defines the scheduling policy for dongle attribution.
 */
typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

/**
 * @brief Holds all the simulation configuration parameters.
 */
typedef struct s_config
{
	int			nb_coders;		// Nombre total de codeurs
	int			nb_compiles;	// Nombre de compilations requises
	size_t		time_burnout;	// Temps avant burnout en ms
	size_t		time_compile;	// Temps de compilation en ms
	size_t		time_debug;		// Temps de debug en ms
	size_t		time_refactor;	// Temps de refactoring en ms
	size_t		cooldown;		// Temps de rechargement du dongle en ms
	t_scheduler	scheduler;		// Politique d'ordonnancement (fifo ou edf)
}	t_config;

/**
 * @brief Defines one dongle state.
 */
typedef struct s_dongle
{
	pthread_mutex_t	is_available;
	size_t			free_time;
	int				dongle_id;
}	t_dongle;

/**
 * @brief Defines one coder (one thread).
 */
typedef struct s_coder
{
	int			coder_id;
	pthread_t	thread_id;
	t_dongle	*dgl_adj[2];
	size_t		last_compile;
	int			count_compile;
	t_config	*config;
}	t_coder;

/**
 * @brief Hold all the simulation engine.
 */
typedef struct s_sim
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	bool			stop;
	pthread_mutex_t	can_display;
	pthread_mutex_t	can_stop;
}	t_sim;

#endif