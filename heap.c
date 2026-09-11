/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 09:06:24 by jfoeller          #+#    #+#             */
/*   Updated: 2026/09/11 13:20:13 by jfoeller         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief Inserts a new request into the min-heap.
 * 
 * @param heap Pointer to the heap.
 * @param coder Pointer to the coder making the request.
 * @param priority The priority value of the request.
 * @return true if inserted, false if the heap is full.
 */
bool	insert_request(t_heap *heap, t_coder *coder, size_t priority)
{
	if (heap->nb_requests < heap->max_requests)
	{
		heap->requests[heap->nb_requests].coder = coder;
		heap->requests[heap->nb_requests].priority_value = priority;
		heap->nb_requests++;
		return (true);
	}
	return (false);
}

/**
 * @brief Swaps two requests in the heap array.
 * 
 * @param a Pointer to the first request.
 * @param b Pointer to the second request.
 */
void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/**
 * @brief Moves a request up the heap to restore the min-heap property.
 * 
 * @param heap Pointer to the heap.
 * @param index The index of the newly inserted request.
 */
void	bubble_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = ((index - 1) / 2);
		if (heap->requests[index].priority_value
			< heap->requests[parent].priority_value)
		{
			swap_requests(&heap->requests[index],
				&heap->requests[parent]);
			index = parent;
		}
		else
			index = 0;
	}
}

/**
 * @brief Moves a request down the heap to restore the min-heap property.
 * 
 * @param heap Pointer to the heap.
 * @param index The index of the request to bubble down.
 */
void	bubble_down(t_heap *heap, int index)
{
	int	child_1;
	int	child_2;
	int	smallest;

	while ((2 * index) + 1 < heap->nb_requests)
	{
		child_1 = (2 * index) + 1;
		child_2 = (2 * index) + 2;
		smallest = child_1;
		if (child_2 < heap->nb_requests
			&& heap->requests[child_2].priority_value
			< heap->requests[child_1].priority_value)
			smallest = child_2;
		if (heap->requests[index].priority_value
			> heap->requests[smallest].priority_value)
		{
			swap_requests(&heap->requests[index], &heap->requests[smallest]);
			index = smallest;
		}
		else
			index = heap->max_requests;
	}
}

/**
 * @brief Extracts the request with the highest priority
 * (lowest value) from the heap.
 * 
 * @param heap Pointer to the heap.
 * @return The extracted request, or a request with a
 * NULL coder if the heap is empty.
 */
t_request	extract_min(t_heap *heap)
{
	t_request	min_req;
	t_request	empty_req;

	empty_req.coder = NULL;
	if (heap->nb_requests == 0)
		return (empty_req);
	min_req = heap->requests[0];
	heap->requests[0] = heap->requests[heap->nb_requests - 1];
	heap->nb_requests--;
	bubble_down(heap, 0);
	return (min_req);
}
