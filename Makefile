HOST=$(HOSTTYPE)

ifeq ($(HOST),)
	HOST := $(shell uname -m)_$(shell uname -s)
else
	HOST := $(HOSTTYPE)
endif

COMPILER=cc
FLAGS=-Wall -Wextra -Werror -fPIC

all :
	@echo my HOSTTYPE is $(HOST)
#	cc *.c

clean :

fclean :

re : fclean all

.PHONY : all clean fclean re