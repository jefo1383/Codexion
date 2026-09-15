# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jfoeller <jeremy.foeller@learner.42.tec    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/31 13:47:44 by jfoeller          #+#    #+#              #
#    Updated: 2026/09/15 15:11:16 by jfoeller         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= codexion
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
HEADER	= codexion.h
SRCS	= heap.c init.c main.c parsing.c routine_utils.c routine.c utils_bis.c utils.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re