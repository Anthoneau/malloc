#ifndef _MALLOC_H
# define _MALLOC_H

# include <stdlib.h>
# include <sys/mman.h>
# include <unistd.h>

void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void *ptr);

#endif