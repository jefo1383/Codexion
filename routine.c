/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 17:43:21 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/03 18:11:36 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
}
