HOST		=	$(HOSTTYPE)

ifeq ($(HOST),)
	HOST	:=	$(shell uname -m)_$(shell uname -s)
else
	HOST	:=	$(HOSTTYPE)
endif

#BASE

NAME		=	libft_malloc_$(HOST).so
SYMB		=	libft_malloc.so
LNNAME		=	ft_malloc
LIBFTDOR	=	libft/
LIBFT		=	libft.a
CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -fPIC
LDFLAGS		=	-shared
SYMLINK		=	ln -s
SRCS		=	malloc.c
RM			=	rm -rf
SRCS_DIR	=	srcs/
INCL_DIR	=	includes/
OBJS_DIR	=	objs/

#SOURCES

SRCS_FILES	=	malloc
INCL_FILES	=	malloc.h

SRCS 		= 	${addprefix ${SRCS_DIR}, ${addsuffix .c, ${SRCS_FILES}}}
OBJS 		= 	${addprefix ${OBJS_DIR}, ${addsuffix .o, ${SRCS_FILES}}}

#COLORS

BGREEN		=	\033[1;32m
BBLUE		=	\033[1;34m
BCYAN		=	\033[1;36m
COLOR_END	=	\033[0m

# ---------------------------------------------------------------------------- #

MAKEFLAGS	+=	--no-print-directory
OBJSF		=	.cache_exists

all:		${SYMB}

${SYMB}:	${NAME}
			@${SYMLINK} ${NAME} ${SYMB}
			@echo "${BGREEN}Malloc compiled!${COLOR_END}"

#${NAME}:	${LIBFT} | ${OBJS}
${NAME}:	${OBJS}
			@${CC} ${LDFLAGS} ${OBJS} -o ${NAME}

${OBJS_DIR}%.o :	${SRCS_DIR}%.c | ${OBJSF}
			@mkdir -p $(dir $@)
			@${CC} ${CFLAGS} -c $< -o $@

${OBJSF}:
			@mkdir -p ${OBJS_DIR}

#${LIBFT}:
#			@make -C libft/
#			@make clean -C libft/

clean:
			@${RM} ${OBJS_DIR}
			@${RM} ${OBJSF}
			@echo "${BCYAN}Malloc .o files cleaned!${COLOR_END}"

fclean:		clean
			@${RM} ${LIBFT}
			@${RM} ${SYMB}
			@${RM} ${NAME}
			@make fclean -C libft/
			@echo "${BBLUE}Malloc files cleaned!${COLOR_END}"

re:			fclean all

run:		all
			@cc main.c -o test -L. -l${LNNAME}
			@LD_LIBRARY_PATH=. ./test

.PHONY:		all clean fclean re