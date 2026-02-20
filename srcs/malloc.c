#include "../includes/malloc.h"
#include <stdio.h>

void *malloc(size_t size) {
	printf("salut, voici size: %zu\n", size);
	return NULL;
}