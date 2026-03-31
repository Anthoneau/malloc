#include "../includes/malloc.h"
//#include <stdio.h>

t_alloc g_alloc;

void *malloc(size_t size) {
	// check size <= 0 avec le vrai malloc
	//void *c = sbrk(size);
	//return c == (void*) -1 ? NULL : c;
	//write(1, "grooooooos\n", size);
	//size_t salut = sysconf(_SC_PAGESIZE);
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	return ptr == MAP_FAILED ? NULL : ptr; // MAP_FAILED = (void *) -1
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