#ifndef _MALLOC_H
# define _MALLOC_H

# include <stdlib.h>
# include <stdint.h>
# include <sys/mman.h>
# include <unistd.h>
# include <pthread.h>
# include "libft.h"

# if defined(__linux__)
	#define pagesize sysconf(_SC_PAGESIZE)
# elif defined(__APPLE__)
	#define pagesize getpagesize()
# endif

# define ALIGN(size) (((size) + 15) & ~15)

# define TINY_SIZE 128
# define TINY_MAX (100 * (sizeof(t_chunk) + TINY_SIZE))

# define SMALL_SIZE 4096
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
	size_t			align_size;
	size_t			req_size;
	struct s_chunk	*next;
	struct s_chunk	*prev;
	void			*_padding; // 40 à 48, aligné à 16 avec _padding
}	t_chunk;

typedef struct s_zone
{
	size_t			size;
	size_t			size_available;
	size_t			n_of_chunks;
	t_chunk			*chunk;
	t_type			type;
	struct s_zone	*next;
	struct s_zone	*prev;
	void			*_padding; // 56 à 64
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
void show_alloc_mem(void);

#endif