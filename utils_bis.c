/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bis.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 14:14:04 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/14 14:20:10 by jfoeller         ###   ########.fr       */
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
