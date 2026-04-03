#include "../includes/malloc.h"

static t_alloc g_alloc;

t_chunk *alloc_zone(size_t size, t_zone **zone, t_type type) {
	int prot_flag = PROT_READ | PROT_WRITE;
	int map_flag = MAP_PRIVATE | MAP_ANONYMOUS;
	int type_arr[3] = {
		TINY_MAX,
		SMALL_MAX,
		size
	};
	size_t zone_size = MULTIPLE(type_arr[type]);

	(*zone) = (t_zone *)mmap(NULL, zone_size, prot_flag, map_flag, -1, 0);
	if ((*zone) == MAP_FAILED)
		return NULL;

	(*zone)->size = zone_size;
	(*zone)->next = NULL;
	(*zone)->chunk = (t_chunk *)((*zone) + 1);

	t_chunk *chunk = (*zone)->chunk;
	chunk->used = 1;
	chunk->size = ALIGN(size);
	chunk->real_size = size;
	chunk->prev = NULL;
	
	chunk->next = (t_chunk *)((char *)(chunk + 1) + (chunk->size));
	chunk->next->prev = chunk;
	chunk = chunk->next;
	chunk->used = 0;
	chunk->size = (*zone)->size - (sizeof(t_chunk) + (*zone)->chunk->size);
	chunk->real_size = chunk->size;
	chunk->next = NULL;

	(*zone)->size_available = (*zone)->size - (sizeof(t_chunk) + (*zone)->chunk->size);
	(*zone)->n_of_chunks = 2;

	return ((*zone)->chunk);
}

t_chunk *alloc_chunk(size_t size, t_chunk *current) {
	size_t r_size = ALIGN(size);
	while (current) {
		if (current->used == 0 && current->size >= r_size)
			break ;
		if (current->next)
			current = current->next;
		else
			break ;
	}

	size_t free_size = current->size;
	current->used = 1;
	current->size = r_size;
	current->real_size = size;
	t_chunk *free_chunk = (t_chunk *)((char *)(current + 1) + current->size);
	free_chunk->used = 0;
	free_chunk->size = free_size - (sizeof(t_chunk) + r_size);
	free_chunk->next = NULL;
	current->next = free_chunk;
	free_chunk->prev = current;

	return (current);
}

int check_availability(size_t size, t_zone *zone) {
	size_t r_size = ALIGN(size);
	t_chunk *current = zone->chunk;

	if (zone->size_available < r_size)
		return 0;
	while (current) {
		if (current->used == 0 && current->size >= r_size)
			return 1;
		else if (current->next)
			current = current->next;
		else
			break ;
	}
	return 0;
}

void *do_alloc(size_t size, t_zone **zone, t_type type) {
	t_chunk *chunk = NULL;
	while ((*zone)) { // boucle pour atteindre la prochaine zone valide
		if (type != LARGE && check_availability(size, *zone))
			break ;
		(*zone) = (*zone)->next;
	}

	if ((*zone) == NULL) // on crée une zone
		chunk = alloc_zone(size, zone, type);
	else { // la zone contient de la place
		chunk = alloc_chunk(size, (*zone)->chunk);
		(*zone)->size_available -= (sizeof(t_chunk) + chunk->size);
		(*zone)->n_of_chunks++;
	}
	return ((chunk == NULL) ? NULL : ((void *)(chunk + 1)));
}

void *malloc(size_t size) {
	if (size <= TINY_SIZE)
		return do_alloc(size, &g_alloc.tiny, TINY);
	else if (size <= SMALL_SIZE)
		return do_alloc(size, &g_alloc.small, SMALL);
	else
		return do_alloc(size, &g_alloc.large, LARGE);
	return NULL;
}

void defragmentation(t_chunk *chunk) {
	t_chunk *next = chunk->next;
	chunk->size += sizeof(t_chunk) + next->size;
	if (next->next)
		next->next->prev = chunk;
	chunk->next = next->next;
}

void free(void *ptr) {
	/*
		TODO
		Il manque munmap(), je dois encore checker si une zone est completement vide ou non
		Les tailles de t_chunk et de t_zone sont égales à 40 donc je pense que je peux "simplement"
		bondir de 40 octets en 40 octets jusqu'à trouver t_zone.
		De là, je peux accéder à la size_available pour pouvoir la décrémenter ET checker si je dois munmap().
	*/
	if (!ptr)
		return ;
	t_chunk *chunk;
	chunk = ptr - sizeof(t_chunk);
	if (!chunk)
		return ;
	chunk->used = 0;
	if (chunk->next && chunk->next->used == 0)
		defragmentation(chunk);
	if (chunk->prev && chunk->prev->used == 0)
		defragmentation(chunk->prev);
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
	(void)ptr;
	write(1, "show alloc\n", 12);
}