#include "../includes/malloc.h"

static t_alloc g_alloc;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

// MALLOC

static void create_first_chunks(size_t size, t_zone **zone, size_t size_available)
{
	t_chunk *chunk = (*zone)->chunk;
	chunk->used = 1;
	chunk->align_size = ALIGN(size);
	chunk->req_size = size;
	chunk->next = NULL;
	chunk->prev = NULL;

	chunk->next = (t_chunk *)((char *)(chunk + 1) + (chunk->align_size));
	chunk->next->prev = chunk;
	chunk = chunk->next;
	chunk->used = 0;
	chunk->align_size = size_available;
	chunk->req_size = 0;
	chunk->next = NULL;
}

static t_chunk *alloc_zone(size_t size, t_zone **zone, t_type type)
{
	int prot_flag = PROT_READ | PROT_WRITE;
	int map_flag = MAP_PRIVATE | MAP_ANONYMOUS;
	size_t type_arr[3] = { TINY_MAX, SMALL_MAX, sizeof(t_zone) + 2 * sizeof(t_chunk) + ALIGN(size) };
	size_t zone_size = MULTIPLE(type_arr[type]);

	t_zone *new = (t_zone *)mmap(NULL, zone_size, prot_flag, map_flag, -1, 0);
	if (new == MAP_FAILED)
		return NULL;
	(*zone) = new;

	(*zone)->size = zone_size;
	(*zone)->next = NULL;
	(*zone)->chunk = (t_chunk *)((*zone) + 1);

	size_t size_available = (*zone)->size - (sizeof(t_zone) + 2 * sizeof(t_chunk) + ALIGN(size));

	create_first_chunks(size, zone, size_available);

	(*zone)->size_available = size_available;
	(*zone)->n_of_chunks = 2;
	(*zone)->type = type;

	return ((*zone)->chunk);
}

static void fragment_chunk(t_chunk *current, size_t free_size, size_t r_size) {
	t_chunk *free_chunk = (t_chunk *)((char *)(current + 1) + current->align_size);
	free_chunk->used = 0;
	free_chunk->align_size = free_size - (sizeof(t_chunk) + r_size);
	free_chunk->req_size = 0;
	free_chunk->next = current->next;
	free_chunk->prev = current;
	if (free_chunk->next)
		free_chunk->next->prev = free_chunk;
	current->next = free_chunk;
}

static t_chunk *alloc_chunk(size_t size, t_chunk *current, t_zone *zone)
{
	size_t size_aligned = ALIGN(size);
	while (current)
	{
		if (current->used == 0 && current->align_size >= size_aligned)
			break ;
		if (current->next)
			current = current->next;
		else
			break ;
	}

	size_t free_size = current->align_size;
	current->used = 1;
	current->align_size = size_aligned;
	current->req_size = size;
	if	(free_size != size_aligned && free_size > (sizeof(t_chunk) + size_aligned) &&
		(free_size - (sizeof(t_chunk) + size_aligned)) > sizeof(t_chunk))
	{
		fragment_chunk(current, free_size, size_aligned);
		zone->n_of_chunks++;
	}
	else
		current->align_size = free_size;

	return (current);
}

static int check_availability(size_t size, t_zone *zone)
{
	size_t size_aligned = ALIGN(size);
	t_chunk *current = zone->chunk;

	if (zone->size_available < (size_aligned + sizeof(t_chunk)))
		return 0;
	while (current)
	{
		if (current->used == 0 && current->align_size >= size_aligned)
			return 1;
		else if (current->next)
			current = current->next;
		else
			break ;
	}
	return 0;
}

static void *do_alloc(size_t size, t_zone **g_zone, t_type type)
{
	t_zone *zone = *g_zone;
	t_zone *prev = NULL;
	while (zone)
	{
		if (type != LARGE && check_availability(size, zone))
			break ;
		prev = zone;
		zone = zone->next;
	}

	t_chunk *chunk = NULL;
	if (zone == NULL)
	{
		chunk = alloc_zone(size, (prev == NULL) ? g_zone : &prev->next, type);
		if (!chunk)
			return NULL;
		if (prev == NULL)
			(*g_zone)->prev = NULL;
		else
			prev->next->prev = prev;
	}
	else
	{
		chunk = alloc_chunk(size, zone->chunk, zone);
		if (zone->size_available >= sizeof(t_chunk) + chunk->align_size)
			zone->size_available -= sizeof(t_chunk) + chunk->align_size;
		else
			zone->size_available = 0;
	}
	return ((void *)(chunk + 1));
}

void *malloc(size_t size)
{
	if (size > SIZE_MAX - (sizeof(t_zone) + 2 * sizeof(t_chunk) + pagesize))
		return NULL;
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

static void defragmentation(t_chunk *chunk, t_zone *zone)
{
	t_chunk *next = chunk->next;
	chunk->align_size += sizeof(t_chunk) + next->align_size;
	if (next->next)
		next->next->prev = chunk;
	chunk->next = next->next;
	zone->n_of_chunks--;
	zone->size_available += sizeof(t_chunk);
}

static void unset_zone(t_zone *zone)
{
	t_zone **head[3] = { &g_alloc.tiny, &g_alloc.small, &g_alloc.large };
	t_zone *prev = zone->prev;
	t_zone *next = zone->next;

	if (!prev)
		*head[zone->type] = next;
	else
		prev->next = next;
	if (next)
		next->prev = prev;
}

static t_chunk *find_chunk(void *ptr, t_zone **out)
{
	t_zone *zones[3] = { g_alloc.tiny, g_alloc.small, g_alloc.large };

	for (int i = 0; i < 3; i++)
		for (t_zone *z = zones[i]; z; z = z->next)
		{
			if ((char *)ptr < (char *)z || (char *)ptr >= (char *)z + z->size)
				continue ;
			for (t_chunk *c = z->chunk; c; c = c->next)
				if ((void *)(c + 1) == ptr && c->used)
					return (*out = z, c);
			return NULL;
		}
	return NULL;
}

void free(void *ptr)
{
	t_zone *zone;
	t_chunk *chunk;

	if (!ptr)
		return ;
	pthread_mutex_lock(&g_mutex);
	chunk = find_chunk(ptr, &zone);
	if (!chunk)
	{
		pthread_mutex_unlock(&g_mutex);
		return ;
	}

	chunk->used = 0;
	chunk->req_size = 0;
	zone->size_available += chunk->align_size;
	
	while (chunk->next && chunk->next->used == 0)
		defragmentation(chunk, zone);
	while (chunk->prev && chunk->prev->used == 0)
	{
		defragmentation(chunk->prev, zone);
		chunk = chunk->prev;
	}
	
	if (zone->n_of_chunks == 1 && zone->chunk->used == 0
		&& (zone->type == LARGE || zone->prev || zone->next))
	{
		unset_zone(zone);
		munmap(zone, zone->size);
	}
	pthread_mutex_unlock(&g_mutex);
}

// REALLOC

static void *param_edge_case(void *ptr, size_t size)
{
	if (!ptr)
	{
		pthread_mutex_unlock(&g_mutex);
		return (malloc(size));
	}
	pthread_mutex_unlock(&g_mutex);
	free(ptr);
	return NULL;
}

static void create_double_chunk(size_t size, t_chunk *c, t_zone *z)
{
	size_t total_size = c->align_size;
	c->align_size = size;
	t_chunk *new = (t_chunk *)((char *)(c + 1) + (c->align_size));
	new->next = c->next;
	new->prev = c;
	c->next = new;
	if (new->next)
		new->next->prev = new;
	new->used = 0;
	new->align_size = total_size - (c->align_size + sizeof(t_chunk));
	new->req_size = 0;
	z->n_of_chunks++;
	z->size_available += new->align_size;
}

static void chunk_merge(size_t size, t_chunk *c, t_zone *z)
{
	size_t new_size = c->align_size + c->next->align_size + sizeof(t_chunk);
	c->align_size = new_size;
	t_chunk *next = c->next;
	c->next = next->next;
	if (next->next)
		next->next->prev = c;
	z->n_of_chunks--;
	if (z->size_available >= next->align_size + sizeof(t_chunk))
		z->size_available -= next->align_size + sizeof(t_chunk);
	else
		z->size_available = 0;

	if (c->align_size > size && c->align_size - size > sizeof(t_chunk))
		create_double_chunk(size, c, z);
}

void *realloc(void *ptr, size_t requested_size)
{
	pthread_mutex_lock(&g_mutex);
	
	if (!ptr || requested_size == 0)
		return param_edge_case(ptr, requested_size);
		
	size_t size = ALIGN(requested_size);
	
	t_zone *zone;
	t_chunk *chunk = find_chunk(ptr, &zone);
	if (!chunk || chunk->used == 0)
	{
		pthread_mutex_unlock(&g_mutex);
		return NULL;
	}

	if (size <= chunk->align_size)
	{
		if (chunk->align_size - size > sizeof(t_chunk))
			create_double_chunk(size, chunk, zone);
		chunk->req_size = requested_size;
		pthread_mutex_unlock(&g_mutex);
		return ptr;
	}
	if (size > chunk->align_size &&
		chunk->next && chunk->next->used == 0 &&
		size <= chunk->align_size + chunk->next->align_size + sizeof(t_chunk))
	{
		chunk_merge(size, chunk, zone);
		chunk->req_size = requested_size;
		pthread_mutex_unlock(&g_mutex);
		return ptr;
	}

	pthread_mutex_unlock(&g_mutex);
	void *new = malloc(requested_size);
	if (!new)
		return (NULL);
	pthread_mutex_lock(&g_mutex);
	ft_memcpy(new, ptr, chunk->req_size);
	pthread_mutex_unlock(&g_mutex);
	free(ptr);
	return new;
}

// SHOW_ALLOC_MEM

static void print_zone(char *zone, unsigned long adr)
{
	ft_putstr_fd(zone, 1);
	ft_putstr_fd(" : ", 1);
	ft_putaddress_fd(adr);
	ft_putchar_fd('\n', 1);
}

static size_t print_chunk(t_chunk *chunk)
{
	if (chunk->used == 0)
		return 0;
	unsigned long begin = (unsigned long)(chunk + 1);
	unsigned long end = begin + chunk->req_size;

	ft_putaddress_fd(begin);
	ft_putstr_fd(" - ", 1);
	ft_putaddress_fd(end);
	ft_putstr_fd(" : ", 1);
	ft_putsize_t_fd(chunk->req_size, 1);
	ft_putendl_fd(" bytes", 1);

	return (chunk->req_size);
}

static void print_total(int total)
{
	ft_putstr_fd("Total : ", 1);
	ft_putsize_t_fd(total, 1);
	ft_putendl_fd(" bytes", 1);
}

static size_t print_zone_list(char *name, t_zone *head)
{
	t_zone *last = NULL;
	size_t total = 0;

	while (1)
	{
		t_zone *next = NULL;
		for (t_zone *z = head; z; z = z->next)
			if ((!last || z > last) && (!next || z < next))
				next = z;
		if (!next)
			break ;
		print_zone(name, (unsigned long)next);
		for (t_chunk *c = next->chunk; c; c = c->next)
			total += print_chunk(c);
		last = next;
	}
	return (total);
}

void show_alloc_mem(void)
{
	char *names[3] = { "TINY", "SMALL", "LARGE" };
	t_zone *zones[3];
	size_t total = 0;

	pthread_mutex_lock(&g_mutex);
	zones[0] = g_alloc.tiny;
	zones[1] = g_alloc.small;
	zones[2] = g_alloc.large;
	for (int i = 0; i < 3; i++)
		total += print_zone_list(names[i], zones[i]);
	print_total(total);
	pthread_mutex_unlock(&g_mutex);
}
