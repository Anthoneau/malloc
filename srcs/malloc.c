#include "../includes/malloc.h"

static t_alloc g_alloc;
static int i = 0;

void *malloc(size_t size) {
	// check size <= 0 avec le vrai malloc
	//void *c = sbrk(size);
	//return c == (void*) -1 ? NULL : c;
	//write(1, "grooooooos\n", size);
	//size_t salut = sysconf(_SC_PAGESIZE);

	//void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	//return ptr == MAP_FAILED ? NULL : ptr; // MAP_FAILED = (void *) -1

	int prot_flag = PROT_READ | PROT_WRITE;
	int map_flag = MAP_PRIVATE | MAP_ANONYMOUS;

	write(1, "my_malloc\n", 11);

	if (size <= TINY_SIZE) {
		t_zone **tiny = &g_alloc.tiny;
		while ((*tiny) && (*tiny)->next)
			(*tiny) = (*tiny)->next;
		write(1, "tiny malloc\n", 13);
		if ((*tiny) == NULL) {
			write(1, "tiny DONT exist\n", 17);
			(*tiny) = (t_zone *)mmap(NULL, pagesize, prot_flag, map_flag, -1, 0);
			if ((*tiny) == MAP_FAILED) {
				write(1, "error\n", 7);
				return NULL;
			}
			//ft_printf("adress of (*tiny) : %p\n", (*tiny));
			(*tiny)->size = pagesize;
			(*tiny)->next = NULL;
			(*tiny)->chunk = (t_chunk *)&(*tiny) + sizeof(t_zone) + 1;
			//ft_printf("adress of (*tiny)->chunk : %p\n", (*tiny)->chunk);
			write(1, "success for t_zone\n", 20);
			(*tiny)->chunk->used = 0;
			(*tiny)->chunk->size = ALIGN(size);
			(*tiny)->chunk->next = NULL;
			(*tiny)->chunk->prev = NULL;
			(*tiny)->size_available = (*tiny)->size - (*tiny)->chunk->size;
			return (void *)(&(*tiny)->chunk + sizeof(t_chunk));
		}
		else if ((*tiny)->size_available >= ALIGN(size)) {
			write(1, "tiny exist\n", 12);
			t_chunk *current = (*tiny)->chunk;
			while (current && current->next)
				current = current->next;
			current->next = (t_chunk *)&current + sizeof(t_chunk) + current->size + 1;
			current->next->prev = current;
			current = current->next;
			current->used = 0;
			current->size = ALIGN(size);
			current->next = NULL;
			(*tiny)->size_available -= current->size;
			return (void *)(current + sizeof(t_chunk));
		}
		else {
			if (i == 2)
				return NULL;
			write(1, "tiny exist but is full\n", 24);
			(*tiny)->next = (t_zone *)mmap(NULL, pagesize, prot_flag, map_flag, -1, 0);
			if ((*tiny) == MAP_FAILED) {
				write(1, "error\n", 7);
				return NULL;
			}
			(*tiny) = (*tiny)->next;
			//ft_printf("adress of (*tiny) : %p\n", (*tiny));
			(*tiny)->size = pagesize;
			(*tiny)->next = NULL;
			(*tiny)->chunk = (t_chunk *)&(*tiny) + sizeof(t_zone) + 1;
			//ft_printf("adress of (*tiny)->chunk : %p\n", (*tiny)->chunk);
			write(1, "success for t_zone\n", 20);
			(*tiny)->chunk->used = 0;
			(*tiny)->chunk->size = ALIGN(size);
			(*tiny)->chunk->next = NULL;
			(*tiny)->chunk->prev = NULL;
			(*tiny)->size_available = (*tiny)->size - (*tiny)->chunk->size;
			i++;
			return (void *)(&(*tiny)->chunk + sizeof(t_chunk));
		}
	}
	return NULL;
}

void free(void *ptr) {
	(void)ptr;
	write(1, "free\n", 6);
}

void *realloc(void *ptr, size_t size) {
	//printf("adress: %p\n", ptr);
	//printf("size: %zu\n", size);
	(void)ptr;
	(void)size;
	write(1, "realloc\n", 9);
	return NULL;
}

void show_alloc_mem(void *ptr) {
	//printf("adress: %p\n", ptr);
	(void)ptr;
	write(1, "show alloc\n", 12);
}