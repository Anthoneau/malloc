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
	if (free_size != r_size && (free_size - (sizeof(t_chunk) + r_size)) >= sizeof(t_chunk)) {
		t_chunk *free_chunk = (t_chunk *)((char *)(current + 1) + current->size);
		free_chunk->used = 0;
		free_chunk->size = free_size - (sizeof(t_chunk) + r_size);
		free_chunk->next = NULL;
		current->next = free_chunk;
		free_chunk->prev = current;
	}
	else
		current->size = free_size;

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

void *do_alloc(size_t size, t_zone **g_zone, t_type type) {
	t_zone *zone = *g_zone;
	t_zone *prev = NULL;
	while (zone) { // boucle pour atteindre la prochaine zone valide
		if (type != LARGE && check_availability(size, zone))
			break ;
		prev = zone;
		zone = zone->next;
	}

	t_chunk *chunk = NULL;
	if (zone == NULL) { // on crée une zone
		if (prev == NULL)
			chunk = alloc_zone(size, g_zone, type);
		else
			chunk = alloc_zone(size, &prev->next, type);
	}
	else { // la zone contient de la place
		chunk = alloc_chunk(size, zone->chunk);
		zone->size_available -= (sizeof(t_chunk) + chunk->size);
		zone->n_of_chunks++;
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

void defragmentation(t_chunk *chunk, t_zone *zone) {
	t_chunk *next = chunk->next;
	chunk->size += sizeof(t_chunk) + next->size;
	if (next->next)
		next->next->prev = chunk;
	chunk->next = next->next;
	zone->n_of_chunks--;
}

t_zone *find_zone(t_chunk *chunk) {
	t_chunk *temp = chunk;
	while (temp && temp->prev)
		temp = temp->prev;
	return (t_zone *)((temp - 1));
}

void free(void *ptr) {
	if (!ptr)
		return ;
	t_chunk *chunk = (t_chunk *)ptr - 1;
	if (!chunk)
		return ;

	chunk->used = 0;
	t_zone *zone = find_zone(chunk);
	
	while (chunk->next && chunk->next->used == 0)
		defragmentation(chunk, zone);
	while (chunk->prev && chunk->prev->used == 0)
		defragmentation(chunk->prev, zone);
	
	if (zone->n_of_chunks == 1 && zone->chunk->used == 0)
		munmap(zone, zone->size);
}

void *realloc(void *ptr, size_t size) {
	//printf("adress: %p\n", ptr);
	//printf("size: %zu\n", size);
	(void)ptr;
	(void)size;
	write(1, "realloc\n", 9);
	return NULL;
}

void print_zone(char *zone, unsigned long adr) {
	ft_putstr_fd(zone, 1);
	ft_putstr_fd(" : ", 1);
	ft_putaddress_fd(adr);
}

void show_alloc_mem(void) {
	ft_printf("show alloc mem\n");
	//ft_putendl_fd("adresse de tiny", 1);
	//unsigned long adr = (unsigned long)&g_alloc.tiny;
	//ft_putaddress_fd(adr);
	unsigned long adr = (unsigned long)&g_alloc.tiny;
	print_zone("TINY", adr);
	ft_putchar_fd('\n', 1);
	adr = (unsigned long)&g_alloc.small;
	print_zone("SMALL", adr);
	ft_putchar_fd('\n', 1);
	adr = (unsigned long)&g_alloc.large;
	print_zone("LARGE", adr);
	ft_putchar_fd('\n', 1);
	write(1, "show alloc\n", 12);
}

/*
int type_arr[3] = {
	TINY_MAX,
	SMALL_MAX,
	size
};


TINY : 0xA0000
0xA0020 - 0xA004A : 42 bytes
0xA006A - 0xA00BE : 84 bytes
SMALL : 0xAD000
0xAD020 - 0xADEAD : 3725 bytes
LARGE : 0xB0000
0xB0020 - 0xBBEEF : 48847 bytes
Total : 52698 bytes
*/