#include "../includes/malloc.h"
#include <stdio.h>

void free(void *ptr) {
	printf("adress: %p\n", ptr);
	return NULL;
}