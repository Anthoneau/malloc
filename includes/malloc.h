#ifndef _MALLOC_H
# define _MALLOC_H

# include <stdlib.h>
# include <sys/mman.h>
# include <unistd.h>
# include "libft.h"

# if defined(__linux__)
	#define pagesize sysconf(_SC_PAGESIZE)
# elif defined(__APPLE__)
	#define pagesize getpagesize()
# endif

# define ALIGN(size) (((size) + 15) & ~15)

# define TINY_SIZE 128
# define TINY_MAX (100 * (sizeof(t_chunk) + TINY_SIZE))

# define SMALL_SIZE 1024
# define SMALL_MAX (100 * (sizeof(t_chunk) + SMALL_SIZE))

# define MULTIPLE(size) (((size) + (pagesize - 1)) & ~(pagesize - 1))

typedef enum e_type
{
	TINY,
	SMALL,
	LARGE
}	t_type;

typedef struct s_chunk
{
	int				used;
	size_t			size;
	size_t			real_size;
	struct	s_chunk	*next;
	struct	s_chunk	*prev;
}	t_chunk;

typedef struct s_zone
{
	size_t			size;
	size_t			size_available;
	size_t			n_of_blocs;
	size_t			size_of_biggest;
	t_chunk			*chunk;
	struct s_zone	*next;
}	t_zone;

typedef struct s_alloc
{
	t_zone	*tiny;
	t_zone	*small;
	t_zone	*large;
}	t_alloc;

void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void *ptr);

#endif