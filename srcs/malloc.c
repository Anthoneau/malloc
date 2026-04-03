#include "../includes/malloc.h"

static t_alloc g_alloc;

void *alloc_zone(size_t size, t_zone **zone, t_type type) {
	write(1, "zone DONT exist\n", 17);

	// FLAG
	int prot_flag = PROT_READ | PROT_WRITE;
	int map_flag = MAP_PRIVATE | MAP_ANONYMOUS;

	int type_arr[3] = {
		TINY_MAX,
		SMALL_MAX,
		size
	};

	// MMAP
	(*zone) = (t_zone *)mmap(NULL, MULTIPLE(type_arr[type]), prot_flag, map_flag, -1, 0);
	if ((*zone) == MAP_FAILED)
	{
		write(1, "error\n", 7);
		return NULL;
	}

	// INIT ZONE
	(*zone)->size = pagesize;
	(*zone)->next = NULL;
	(*zone)->chunk = (t_chunk *)((*zone) + 1);

	write(1, "success for t_zone\n", 20);

	/*
		//INIT FIRST CHUNK
	*/
	(*zone)->chunk->used = 0;
	(*zone)->chunk->size = ALIGN(size);
	(*zone)->chunk->next = NULL;
	(*zone)->chunk->prev = NULL;

	/*
		// INIT ZONE MEMBERS
		TODO
		Completion des infos de la zone, c'est encore un brouillon
	*/
	(*zone)->size_available = (*zone)->size - (*zone)->chunk->size;
	(*zone)->n_of_blocs = 2;
	(*zone)->size_of_biggest = (*zone)->size_available;

	// RETURN
	return (void *)((*zone)->chunk + 1);
}

void init_chunk(size_t size, t_chunk *current, t_chunk *new) {
	new = (t_chunk *)((char *)(current + 1) + current->size);
	new->used = 1;
	new->size = ALIGN(size);
	new->real_size = size;
	new->next = NULL;
}

t_chunk *alloc_chunk(size_t size, t_chunk *current) {
	write(1, "zone exist\n", 12);

	// LOOP FOR NEXT FREE CHUNK
	while (current) {
		if (current->used == 0 && current->size >= size)
			break ;
		//if (current->next && current->next->used == 0 && current->next->size >= size)
		//	break ;
		if (current->next)
			current = current->next;
		else
			break ;
	}

	// INIT CHUNK
	//current->next = (t_chunk *)((char *)(current + 1) + current->size);
	//current->next->prev = current;
	//current = current->next;
	//current->used = 0;
	//current->size = ALIGN(size);
	//current->real_size = size;
	//current->next = NULL;
	init_chunk(size, current, current->next);

	// RETURN
	return (current);
}

/*
	TODO
	la fonction ne retourne rien pour l'instant
*/
void do_alloc(size_t size, t_zone **zone, t_type type) {
	/*
		TODO
		boucle pour atteindre la prochaine zone valide
	*/
	while ((*zone) && (*zone)->next) {
		if (size <= (*zone)->size_available && size <= (*zone)->size_of_biggest)
			break ;
		(*zone) = (*zone)->next;
	}

	write(1, "zone malloc\n", 13);

	if ((*zone) == NULL) { //quand la zone n'existe pas encore; premier appel
		alloc_zone(size, zone, type);
	}
	else if ((*zone)->size_available >= ALIGN(size)) { //quand la zone existe
		t_chunk *chunk = alloc_chunk(size, (*zone)->chunk);
		(*zone)->size_available -= chunk->size;
		(*zone)->n_of_blocs++;
		(*zone)->size_of_biggest = (*zone)->size_available;
		//return ((void *)(chunk + 1));
	}
	else { //la zone existe mais est pleine
		write(1, "zone exist but is full\n", 24);
		alloc_zone(size, &(*zone)->next, type);
	}
}

void *malloc(size_t size) {
	write(1, "my_malloc\n", 11);

	if (size <= TINY_SIZE) {
		do_alloc(size, &g_alloc.tiny, TINY);
	}
	else if (size <= SMALL_SIZE) {
		do_alloc(size, &g_alloc.small, SMALL);
	}
	else {
		do_alloc(size, &g_alloc.large, LARGE);
	}
	//if (size <= TINY_SIZE) {
	//	t_zone **tiny = &g_alloc.tiny;
	//	while ((*tiny) && size <= (*tiny)->size_available && (*tiny)->next)
	//		(*tiny) = (*tiny)->next;
	//	write(1, "tiny malloc\n", 13);
	//	if ((*tiny) == NULL) {
	//		write(1, "tiny DONT exist\n", 17);
	//		(*tiny) = (t_zone *)mmap(NULL, MULTIPLE(TINY_MAX), prot_flag, map_flag, -1, 0);
	//		if ((*tiny) == MAP_FAILED) {
	//			write(1, "error\n", 7);
	//			return NULL;
	//		}
	//		//ft_printf("adress of (*tiny) : %p\n", (*tiny));
	//		(*tiny)->size = pagesize;
	//		(*tiny)->next = NULL;
	//		(*tiny)->chunk = (t_chunk *)((*tiny) + 1);
	//		//ft_printf("adress of (*tiny)->chunk : %p\n", (*tiny)->chunk);
	//		write(1, "success for t_zone\n", 20);
	//		(*tiny)->chunk->used = 0;
	//		(*tiny)->chunk->size = ALIGN(size);
	//		(*tiny)->chunk->next = NULL;
	//		(*tiny)->chunk->prev = NULL;
	//		(*tiny)->size_available = (*tiny)->size - (*tiny)->chunk->size;
	//		(*tiny)->n_of_blocs = 2;
	//		(*tiny)->size_of_biggest = (*tiny)->size_available;
	//		return (void *)((*tiny)->chunk + 1);
	//	}
	//	else if ((*tiny)->size_available >= ALIGN(size)) {
	//		write(1, "tiny exist\n", 12);
	//		t_chunk *current = (*tiny)->chunk;
	//		while (current && current->next)
	//			current = current->next;
	//		current->next = (t_chunk *)&current + sizeof(t_chunk) + current->size + 1;
	//		current->next->prev = current;
	//		current = current->next;
	//		current->used = 0;
	//		current->size = ALIGN(size);
	//		current->next = NULL;
	//		(*tiny)->size_available -= current->size;
	//		(*tiny)->n_of_blocs++;
	//		(*tiny)->size_of_biggest = (*tiny)->size_available;
	//		return (void *)(current + 1);
	//	}
	//	else {
	//		write(1, "tiny exist but is full\n", 24);
	//		(*tiny)->next = (t_zone *)mmap(NULL, MULTIPLE(TINY_MAX), prot_flag, map_flag, -1, 0);
	//		if ((*tiny) == MAP_FAILED) {
	//			write(1, "error\n", 7);
	//			return NULL;
	//		}
	//		(*tiny) = (*tiny)->next;
	//		//ft_printf("adress of (*tiny) : %p\n", (*tiny));
	//		(*tiny)->size = pagesize;
	//		(*tiny)->next = NULL;
	//		(*tiny)->chunk = (t_chunk *)&(*tiny) + sizeof(t_zone) + 1;
	//		//ft_printf("adress of (*tiny)->chunk : %p\n", (*tiny)->chunk);
	//		write(1, "success for t_zone\n", 20);
	//		(*tiny)->chunk->used = 0;
	//		(*tiny)->chunk->size = ALIGN(size);
	//		(*tiny)->chunk->next = NULL;
	//		(*tiny)->chunk->prev = NULL;
	//		(*tiny)->size_available = (*tiny)->size - (*tiny)->chunk->size;
	//		(*tiny)->n_of_blocs = 2;
	//		return (void *)((*tiny)->chunk + 1);
	//	}
	//}
	return NULL;
}

void free(void *ptr) {
	if (!ptr)
		return ;
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