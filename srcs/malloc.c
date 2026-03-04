#include "../includes/malloc.h"
//#include <stdio.h>

void *malloc(size_t size) {
	// check size <= 0 avec le vrai malloc
	//void *c = sbrk(size);
	//return c == (void*) -1 ? NULL : c;
	write(1, "grooooooos\n", size);
	int salut = sysconf(size);
	(void)salut;
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