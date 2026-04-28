#include "../includes/malloc.h"

static t_alloc g_alloc;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

// MALLOC

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
	chunk->real_size = 0;
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
	if (free_size != r_size && (free_size - (sizeof(t_chunk) + r_size)) > sizeof(t_chunk)) {
		t_chunk *free_chunk = (t_chunk *)((char *)(current + 1) + current->size);
		free_chunk->used = 0;
		free_chunk->size = free_size - (sizeof(t_chunk) + r_size);
		free_chunk->real_size = 0;
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

	if (zone->size_available < (r_size + sizeof(t_chunk)))
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
	while (zone) {
		if (type != LARGE && check_availability(size, zone))
			break ;
		prev = zone;
		zone = zone->next;
	}

	t_chunk *chunk = NULL;
	if (zone == NULL) {
		if (prev == NULL) {
			chunk = alloc_zone(size, g_zone, type);
			(*g_zone)->prev = NULL;
		}
		else {
			chunk = alloc_zone(size, &prev->next, type);
			prev->next->prev = prev;
		}
	}
	else {
		chunk = alloc_chunk(size, zone->chunk);
		zone->size_available -= (sizeof(t_chunk) + chunk->size);
		if (zone->size_available > zone->size)
			zone->size_available = 0;
		zone->n_of_chunks++;
	}
	return ((chunk == NULL) ? NULL : ((void *)(chunk + 1)));
}

void *malloc(size_t size) {
	pthread_mutex_lock(&g_mutex);
	void *result = NULL;
	if (size <= TINY_SIZE)
		result = do_alloc(size, &g_alloc.tiny, TINY);
	else if (size <= SMALL_SIZE)
		result = do_alloc(size, &g_alloc.small, SMALL);
	else
		result = do_alloc(size, &g_alloc.large, LARGE);
	pthread_mutex_unlock(&g_mutex);
	return result;
}

// FREE

void defragmentation(t_chunk *chunk, t_zone *zone) {
	t_chunk *next = chunk->next;
	chunk->size += sizeof(t_chunk) + next->size;
	if (next->next)
		next->next->prev = chunk;
	chunk->next = next->next;
	zone->n_of_chunks--;
	zone->size_available += sizeof(t_chunk);
}

t_zone *find_zone(t_chunk *chunk) {
	t_chunk *temp = chunk;
	while (temp && temp->prev)
		temp = temp->prev;
	return (t_zone *)((t_zone *)temp - 1);
}

void unset_zone(t_zone *zone) {
	t_zone *prev = zone->prev;
	t_zone *next = zone->next;

	if (!prev) {
		if (zone == g_alloc.tiny)
			g_alloc.tiny = (next) ? next : NULL;
		else if (zone == g_alloc.small)
			g_alloc.small = (next) ? next : NULL;
		else if (zone == g_alloc.large)
			g_alloc.large = (next) ? next : NULL;
	}
	else
		prev->next = next;
	if (next)
		next->prev = prev;
}

void free(void *ptr) {
	if (!ptr)
		return ;
	pthread_mutex_lock(&g_mutex);
	t_chunk *chunk = (t_chunk *)ptr - 1;
	if (!chunk)
		return ;

	chunk->used = 0;
	chunk->real_size = 0;
	t_zone *zone = find_zone(chunk);
	zone->size_available += chunk->size;
	
	while (chunk->next && chunk->next->used == 0)
		defragmentation(chunk, zone);
	while (chunk->prev && chunk->prev->used == 0) {
		defragmentation(chunk->prev, zone);
		chunk = chunk->prev;
	}
	
	if (zone->n_of_chunks == 1 && zone->chunk->used == 0) {
		unset_zone(zone);
		munmap(zone, zone->size);
	}
	pthread_mutex_unlock(&g_mutex);
}

// REALLOC

void *realloc_issue(void *ptr, size_t size) {
	if (!ptr) {
		pthread_mutex_unlock(&g_mutex);
		return (malloc(size));
	}	
	if (size == 0) {
		pthread_mutex_unlock(&g_mutex);
		free(ptr);
		return NULL;
	}
	return NULL;
}

void create_double_chunk(size_t size, t_chunk *chunk, t_zone *zone) {
	size_t total_size = chunk->size;
	chunk->size = size;
	t_chunk *new = (t_chunk *)((char *)(chunk + 1) + (chunk->size));
	new->next = chunk->next;
	chunk->next = new;
	if (new->next)
		new->next->prev = new;
	new->used = 0;
	new->size = total_size - (chunk->size + sizeof(t_chunk));
	zone->n_of_chunks++;
	zone->size_available += new->size;
}

void *ptr_is_free(void* ptr, size_t requested_size, t_chunk *chunk, t_zone *zone) {
	size_t size = ALIGN(requested_size);
	if (chunk->size >= size) {
		chunk->used = 1;
		zone->size_available -= chunk->size + sizeof(t_chunk);
		zone->n_of_chunks++;
		if (chunk->size - size > sizeof(t_chunk))
			create_double_chunk(size, chunk, zone);
		chunk->real_size = requested_size;
		pthread_mutex_unlock(&g_mutex);
		return ptr;
	}
	pthread_mutex_unlock(&g_mutex);
	void *new = malloc(size);
	pthread_mutex_lock(&g_mutex);
	if (!new) {
		chunk->used = 1;
		zone->size_available -= chunk->size + sizeof(t_chunk);
		zone->n_of_chunks++;
		pthread_mutex_unlock(&g_mutex);
		return ptr;
	}
	pthread_mutex_unlock(&g_mutex);
	return (new);
}

void chunk_merge(size_t size, t_chunk *chunk, t_zone *zone) {
	size_t new_size = chunk->size + chunk->next->size + sizeof(t_chunk);
	chunk->size = new_size;
	t_chunk *next = chunk->next;
	chunk->next = next->next;
	if (next->next)
		next->next->prev = chunk;
	zone->n_of_chunks--;
	zone->size_available -= next->size + sizeof(t_chunk);

	if (chunk->size > size && chunk->size - size > sizeof(t_chunk))
		create_double_chunk(size, chunk, zone);
}

void *realloc(void *ptr, size_t requested_size) {
	pthread_mutex_lock(&g_mutex);

	if (!ptr || requested_size == 0)
		return realloc_issue(ptr, requested_size);
	
	size_t size = ALIGN(requested_size);

	t_chunk *chunk = (t_chunk *)ptr - 1;
	t_zone *zone = find_zone(chunk);
	if (chunk->used == 0)
		return ptr_is_free(ptr, requested_size, chunk, zone);

	if (size <= chunk->size) {
		if (chunk->size - size > sizeof(t_chunk))
			create_double_chunk(size, chunk, zone);
		chunk->real_size = requested_size;
		pthread_mutex_unlock(&g_mutex);
		return ptr;
	}
	if (size > chunk->size &&
		chunk->next && chunk->next->used == 0 &&
		size <= chunk->size + chunk->next->size + sizeof(t_chunk)) {
		chunk_merge(size, chunk, zone);
		chunk->real_size = requested_size;
		pthread_mutex_unlock(&g_mutex);
		return ptr;
	}

	pthread_mutex_unlock(&g_mutex);
	void *new = malloc(size);
	if (!new)
		return ptr;
	pthread_mutex_lock(&g_mutex);
	ft_memcpy(new, ptr, chunk->real_size);
	pthread_mutex_unlock(&g_mutex);
	free(ptr);
	return new;
}

// SHOW_ALLOC_MEM

void print_zone(char *zone, unsigned long adr) {
	ft_putstr_fd(zone, 1);
	ft_putstr_fd(" : ", 1);
	ft_putaddress_fd(adr);
	ft_putchar_fd('\n', 1);
}

void print_chunk(t_chunk *chunk) {
	if (chunk->used == 0)
		return ;
	unsigned long begin = (unsigned long)chunk;
	unsigned long end = (unsigned long)(chunk + 1) + chunk->real_size;

	ft_putaddress_fd(begin);
	ft_putstr_fd(" - ", 1);
	ft_putaddress_fd(end);
	ft_putstr_fd(" : ", 1);
	ft_putnbr_fd(chunk->real_size, 1);
	ft_putstr_fd(" - ", 1);
	ft_putnbr_fd(chunk->size, 1);
	ft_putendl_fd(" bytes", 1);
}

void show_alloc_mem(void) {
	char type_arr[3][6] = {
		"TINY",
		"SMALL",
		"LARGE"
	};
	t_zone *zone[3] = {
		g_alloc.tiny,
		g_alloc.small,
		g_alloc.large
	};
	for (int i = 0; i < 3; i++) {
		if (!zone[i]) {
			print_zone(type_arr[i], (unsigned long)zone[i]);
			continue ;
		}
		while (zone[i]) {
			print_zone(type_arr[i], (unsigned long)zone[i]);
			if (zone[i]->chunk) {
				t_chunk *chunk = zone[i]->chunk;
				while (chunk) {
					print_chunk(chunk);
					chunk = chunk->next;
				}
			}
			zone[i] = zone[i]->next;
		}
	}
}
