#include "includes/malloc.h"

int main(void) {
	void *ptr = malloc(5);
	if (!ptr)
		return 1;
	return 0;
}