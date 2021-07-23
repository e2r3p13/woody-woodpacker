# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lfalkau <lfalkau@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/17 02:40:03 by lfalkau           #+#    #+#              #
#    Updated: 2021/07/24 01:16:29 by bccyv            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ****************
#	Variables:

NAME	=	woody_woodpacker

SRCDIR	=	src
INCDIR	=	inc
OBJDIR	=	obj

SRCS	=	chacha20.c				\
			elf64.inject.c			\
			elf64.read.c			\
			elf64.utils.c			\
			elf64.write.c			\
			main.c					\
			# elf64.print.c			\

CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror # -g3 -fsanitize=address

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
	@rm -rf woody
	@printf "[\e[31mCLEAN\e[0m] %s\n" woody
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(NAME)
endif

_clean:
ifeq ($(shell ls -1 | grep $(OBJDIR)),$(OBJDIR))
	@rm -rf $(OBJCTT) $(DPDCTT)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(OBJCTT)
	@rm -rf $(OBJDIR)
endif

re: fclean all
