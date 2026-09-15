/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 14:16:09 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/15 17:17:01 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>

typedef struct s_coder		t_coder;
typedef struct s_request	t_request;
typedef struct s_heap		t_heap;
typedef struct s_sim		t_sim;

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
	bool			in_use;
}	t_dongle;

/**
 * @brief Represents the priority queue (min-heap).
 */
typedef struct s_heap
{
	t_request	*requests;
	int			nb_requests;
	int			max_requests;
}	t_heap;

/**
 * @brief Hold all the simulation engine.
 */
typedef struct s_sim
{
	size_t			start_time;
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	bool			stop;
	pthread_mutex_t	can_display;
	pthread_mutex_t	can_stop;
	pthread_mutex_t	secure_heap;
	pthread_cond_t	wait_heap;
	t_heap			heap;
}	t_sim;

/**
 * @brief Defines one coder (one thread).
 */
typedef struct s_coder
{
	int				coder_id;
	pthread_t		thread_id;
	t_dongle		*dgl_adj[2];
	size_t			last_compile;
	int				count_compile;
	pthread_mutex_t	state_lock;
	t_config		*config;
	t_sim			*sim;
}	t_coder;

/**
 * @brief Represents a single request in the priority queue.
 */
typedef struct s_request
{
	t_coder	*coder;
	size_t	priority_value;
}	t_request;

size_t		get_time_ms(void);
size_t		current_time(t_coder *coder);
bool		check_stop(t_sim *sim);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
size_t		init_request(t_coder *coder);
void		free_all(t_sim *sim, t_heap *heap);
void		rollback_dongles(t_sim *sim, int count);
bool		insert_request(t_heap *heap, t_coder *coder, size_t priority);
void		bubble_up(t_heap *heap, int index);
t_request	extract_min(t_heap *heap);
bool		wait_for_turn(t_coder *coder);
void		wait_both_cooldowns(t_coder *coder);
bool		is_higher_priority(t_request req1, t_request req2);
bool		check_args(int argc, char **argv);
void		init_config(t_config *config, char **argv);
bool		init_sim(t_sim *sim);
void		print_action(t_coder *coder, char *action);
void		print_dongle(t_coder *coder, int dongle_id);
void		*ft_calloc(size_t size, size_t count);
bool		is_burned_out(t_coder *coder);

#endif