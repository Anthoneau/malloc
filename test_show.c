#include <stdlib.h>

void	show_alloc_mem(void);

int	main(void)
{
	void *keep[4];

	keep[0] = malloc(42);
	keep[1] = malloc(84);
	keep[2] = malloc(3725);
	keep[3] = malloc(48847);
	free(malloc(100));
	show_alloc_mem();
	(void)keep;
	return (0);
}