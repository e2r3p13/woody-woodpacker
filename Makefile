# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lfalkau <lfalkau@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/17 02:40:03 by lfalkau           #+#    #+#              #
#    Updated: 2021/07/17 03:37:44 by lfalkau          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ****************
#	Variables:

NAME	=	woody

SRCDIR	=	src
INCDIR	=	inc
OBJDIR	=	obj

SRCS	=	main.c

CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror

OBJS	=	$(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DPDCS	=	$(OBJS:.o=.d)

OBJCTT	=	$(shell ls $(OBJDIR)/*.o)
DPDCTT	=	$(shell ls $(OBJDIR)/*.d)

# ****************
#	Rules:

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf "[\e[32mOK\e[0m] %s\n" $@

-include $(DPDCS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -MMD -I $(INCDIR) -c $< -o $@
	@printf "[\e[32mCC\e[0m] %s\n" $@

clean: _clean

fclean: _clean
ifeq ($(shell ls -1 | grep $(NAME)),$(NAME))
	@rm -rf $(NAME)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(NAME)
endif

_clean:
ifeq ($(shell ls -1 | grep $(OBJDIR)),$(OBJDIR))
	@rm -rf $(OBJCTT) $(DPDCTT)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(OBJCTT)
	@rm -rf $(OBJDIR)
endif

re: fclean all
