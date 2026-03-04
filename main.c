#include "includes/malloc.h"
#include <stdio.h>

int main(void) {
	void *ptr = malloc(12);
	//write(1, "salut\n", 7);
	//printf("sysconf dans le main : %ld\n", sysconf(10));
	if (!ptr){
		write(1, "pointeur nul\n", 14);
		return 1;
	}
	write(1, "pointeur non nul\n", 18);
	return 0;
}