#include "../includes/malloc.h"
#include <stdio.h>

void *realloc(void *ptr, size_t size) {
	printf("adress: %p\n", ptr);
	printf("size: %zu\n", size);
	return NULL;
}