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

.NOTPARALLEL:

# ****************
#	Variables:

NAME	=	woody_woodpacker
PACKED	=	woody
STUB	=	stub

SRCDIR	=	src
INCDIR	=	inc
OBJDIR	=	obj

SRCS	=	chacha20.c				\
			elf64.inject.c			\
			elf64.read.c			\
			elf64.utils.c			\
			elf64.write.c			\
			main.c					\
			utils.c					\
			$(STUB).c				\
			elf64.print.c			\

CC		=	clang -m64
CFLAGS	=	-Wall -Wextra -Werror
DEBUG	=	#-g3 -fsanitize=address

OBJS	=	$(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DPDCS	=	$(OBJS:.o=.d)

OBJCTT	=	$(shell ls $(OBJDIR)/*.o)
DPDCTT	=	$(shell ls $(OBJDIR)/*.d)

# ****************
#	Rules:

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(DEBUG) $(OBJS) -o $(NAME)
	@printf "[\e[32mOK\e[0m] %s\n" $@

-include $(DPDCS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) $(DEBUG) -MMD -I $(INCDIR) -c $< -o $@
	@printf "[\e[32mCC\e[0m] %s\n" $@

$(SRCDIR)/$(STUB).c: $(SRCDIR)/$(STUB).s
	@nasm -f bin -o .$(STUB) $(SRCDIR)/$(STUB).s
	@xxd -i .$(STUB) $(SRCDIR)/$(STUB).c
	@rm -f .$(STUB)
	@sed -i 's/_stub/stub/g' $(SRCDIR)/$(STUB).c

clean: _clean

fclean: _clean
ifeq ($(shell ls -1 | grep -w $(PACKED)), $(PACKED))
	@rm -f $(PACKED)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(PACKED)
endif
ifeq ($(shell ls -1 | grep -w $(NAME)), $(NAME))
	@rm -f $(NAME)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(NAME)
endif

_clean:
ifeq ($(shell ls -1 | grep -w $(OBJDIR)), $(OBJDIR))
	@rm -rf $(OBJCTT) $(DPDCTT)
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(OBJCTT)
	@rm -rf $(OBJDIR)
endif
ifeq ($(shell ls -1 $(SRCDIR) | grep -w $(STUB).c), $(STUB).c)	
	@rm -f $(SRCDIR)/$(STUB).c
	@printf "[\e[31mCLEAN\e[0m] %s\n" $(SRCDIR)/$(STUB).c
endif

re: fclean all
