/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bis.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 14:14:04 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/17 15:14:31 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Compares two requests to determine which has higher priority.
 *        Includes a tie-breaker based on coder_id to ensure determinism.
 *
 * @param req1 The first request to compare.
 * @param req2 The second request to compare against.
 * @return true if req1 has strictly higher priority than req2, false otherwise.
 */
bool	is_higher_priority(t_request req1, t_request req2)
{
	if (req1.priority_value < req2.priority_value)
		return (true);
	else if (req1.priority_value == req2.priority_value)
	{
		if (req1.coder->coder_id < req2.coder->coder_id)
			return (true);
	}
	return (false);
}

/**
 * @brief Displays the coder's state safely.
 * 
 * @param coder Pointer to the coder.
 * @param action String of the action to display.
 */
void	print_action(t_coder *coder, char *action)
{
	pthread_mutex_lock(&coder->sim->can_display);
	if (check_stop(coder->sim))
	{
		pthread_mutex_unlock(&coder->sim->can_display);
		return ;
	}
	printf("%zu %d %s\n", current_time(coder), coder->coder_id, action);
	pthread_mutex_unlock(&coder->sim->can_display);
}

/**
 * @brief Logs the action of taking a dongle safely.
 * 
 * @param coder Pointer to the coder taking the dongle.
 * @param dongle_id The ID of the dongle being taken.
 */
void	print_dongle(t_coder *coder, int dongle_id)
{
	pthread_mutex_lock(&coder->sim->can_display);
	if (check_stop(coder->sim))
	{
		pthread_mutex_unlock(&coder->sim->can_display);
		return ;
	}
	printf("%zu %d has taken dongle %d\n", current_time(coder),
		coder->coder_id, dongle_id);
	pthread_mutex_unlock(&coder->sim->can_display);
}

/**
 * @brief Allocates memory for an array and initializes all bytes to zero.
 * 
 * @param count Number of elements to allocate.
 * @param size Size of each element.
 * @return void* Pointer to the allocated memory, or NULL on failure.
 */
void	*ft_calloc(size_t size, size_t count)
{
	void	*ptr;
	size_t	total;

	total = count * size;
	if (count != 0 && total / count != size)
		return (NULL);
	ptr = malloc(total);
	if (!ptr)
		return (NULL);
	memset(ptr, 0, total);
	return (ptr);
}

/**
 * @brief Checks if a specific coder has burned out.
 * 
 * @param coder Pointer to the coder to evaluate.
 * @return true if the coder has burned out, false otherwise.
 */
bool	is_burned_out(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_lock);
	if ((current_time(coder) - coder->last_compile)
		> coder->sim->config.time_burnout)
	{
		pthread_mutex_unlock(&coder->state_lock);
		pthread_mutex_lock(&coder->sim->can_display);
		printf("%zu %d burned out\n",
			current_time(coder), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->can_display);
		return (true);
	}
	pthread_mutex_unlock(&coder->state_lock);
	return (false);
}
