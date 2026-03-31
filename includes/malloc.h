#ifndef _MALLOC_H
# define _MALLOC_H

# include <stdlib.h>
# include <sys/mman.h>
# include <unistd.h>

# if defined(__linux__)
	#define pagesize sysconf(_SC_PAGESIZE)
# elif defined(__APPLE__)
	#define pagesize getpagesize()
# endif

typedef enum e_size
{
	TINY,
	SMALL,
	LARGE
}	t_size;

typedef struct s_chunk
{
	int		used;
	size_t	size;
	struct	s_chunk *next;
	struct	s_chunk *prev;
}	t_chunk;

typedef struct s_zone
{
	size_t size;
	t_chunk *chunk;
	struct s_zone *next;
}	t_zone;

typedef struct s_alloc
{
	t_zone *tiny;
	t_zone *small;
	t_zone *large;
}	t_alloc;

void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void *ptr);

#endif