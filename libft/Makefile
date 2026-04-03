# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agoldber < agoldber@student.s19.be >       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/27 09:39:13 by agoldber          #+#    #+#              #
#    Updated: 2025/05/19 17:06:03 by agoldber         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#BASE

NAME		=	libft.a
INCLUDES	=	include
SRCS_DIR 	=	src/
OBJS_DIR	=	obj/
CC			=	cc
CFLAGS		=	-Wall -Werror -Wextra -I
RM			=	rm -rf
AR			=	ar -rcs

#COLORS

BGREEN		=	\033[1;32m
BBLUE		=	\033[1;34m
BCYAN		=	\033[1;36m
BPURPLE		=	\033[1;35m
BWHITE		=	\033[1;37m
COLOR_END	=	\033[0m

#SOURCES

FTIS_DIR	=	ft_is/
FTIS		=	ft_isalnum ft_isalpha ft_isascii ft_isdigit ft_isprint ft_isspace ft_isnum

FTMEM_DIR	=	ft_mem/
FTMEM		=	ft_bzero ft_calloc ft_memchr ft_memcmp ft_memmove ft_memset

FTPUT_DIR	=	ft_put/
FTPUT		=	ft_putchar_fd ft_putendl_fd ft_putnbr_fd ft_putstr_fd ft_putaddress_fd

FTTO_DIR	=	ft_to/
FTTO		=	ft_atoi ft_itoa ft_tolower ft_toupper ft_atoi_base

FTSTR_DIR	=	ft_str/
FTSTR		=	ft_split ft_strchr ft_strdup ft_striteri ft_strjoin \
				ft_strlcat ft_strlcpy ft_strlen ft_strmapi ft_strncmp \
				ft_strnstr ft_strrchr ft_strtrim ft_substr

FTPRINTF_DIR=	ft_printf/
FTPRINTF	=	display ft_printf hexa_transfo print_string

GNL_DIR		=	gnl/
GNL			=	get_next_line get_next_line_utils

FTADD_DIR	=	ft_add/
FTADD		=	ft_swap free_array ft_free

SRC_FILES+=${addprefix ${FTIS_DIR},${FTIS}}
SRC_FILES+=${addprefix ${FTMEM_DIR},${FTMEM}}
SRC_FILES+=${addprefix ${FTPUT_DIR},${FTPUT}}
SRC_FILES+=${addprefix ${FTTO_DIR},${FTTO}}
SRC_FILES+=${addprefix ${FTSTR_DIR},${FTSTR}}
SRC_FILES+=${addprefix ${FTPRINTF_DIR},${FTPRINTF}}
SRC_FILES+=${addprefix ${GNL_DIR}, ${GNL}}
SRC_FILES+=${addprefix ${FTADD_DIR}, ${FTADD}}

SRCS 		= 	${addprefix ${SRCS_DIR}, ${addsuffix .c, ${SRC_FILES}}}
OBJS 		= 	${addprefix ${OBJS_DIR}, ${addsuffix .o, ${SRC_FILES}}}

# ---------------------------------------------------------------------------- #

OBJSF		=	.cache_exists
TOTAL_FILES = $(words $(SRCS))

define progress_bar
	@COMPILED=$$(( $(COMPILED_FILES) + 1 )); \
	PROGRESS=$$(($$COMPILED*50/$(TOTAL_FILES))); \
	BAR=$$(for i in `seq 1 $$PROGRESS`; do printf "▇"; done); \
	SPACES=$$(for i in `seq 1 $$((50 - $$PROGRESS))`; do printf " "; done); \
	printf "\r${BBLUE}Compiling: [$$BAR$$SPACES] $$COMPILED/$(TOTAL_FILES) files${COLOR_END}"; \
	echo ""; \
	$(eval COMPILED_FILES=$$COMPILED)
endef

all:		${NAME}

${NAME}:	${OBJS}
			@${AR} ${NAME} ${OBJS}
			@echo "\n${BGREEN}Libft compiled!${COLOR_END}"

COMPILED_FILES=0

${OBJS_DIR}%.o : ${SRCS_DIR}%.c | ${OBJSF}
	@mkdir -p $(dir $@)
	@${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@
	@PROGRESS=$$(echo "$$(find ${OBJS_DIR} -type f | wc -l | tr -d ' ') * 100 / ${TOTAL_FILES}" | bc); \
	BAR=$$(seq -s "▇" 0 $$((PROGRESS / 2)) | tr -d '[:digit:]'); \
	SPACES=$$(seq -s " " 0 $$((50 - PROGRESS / 2)) | tr -d '[:digit:]'); \
	CURRENT_FILES=$$(find ${OBJS_DIR} -type f | wc -l | tr -d ' '); \
	printf "\r${BBLUE}Compiling: [$$BAR$$SPACES] $$PROGRESS%% ($$CURRENT_FILES/${TOTAL_FILES})${COLOR_END}"


${OBJSF}:
			@mkdir -p ${OBJS_DIR}
			@mkdir -p ${OBJS_DIR}${FTIS_DIR}
			@mkdir -p ${OBJS_DIR}${FTMEM_DIR}
			@mkdir -p ${OBJS_DIR}${FTPUT_DIR}
			@mkdir -p ${OBJS_DIR}${FTTO_DIR}
			@mkdir -p ${OBJS_DIR}${FTSTR_DIR}
			@mkdir -p ${OBJS_DIR}${FTLST_DIR}
			@mkdir -p ${OBJS_DIR}${FTPRINTF_DIR}
			@mkdir -p ${OBJS_DIR}${GNL_DIR}
			@mkdir -p ${OBJS_DIR}${FTADD_DIR}

clean:
			@${RM} ${OBJS_DIR}
			@${RM} ${OBJSF}
			@echo "${BCYAN}Libft .o files cleaned!${COLOR_END}"

fclean:		clean
			@${RM} ${NAME}
			@echo "${BBLUE}Libft .a files cleaned!${COLOR_END}"

re:			fclean all

.PHONY:		all clean fclean re