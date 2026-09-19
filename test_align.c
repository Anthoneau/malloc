#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int	main(void)
{
	size_t	sizes[] = { 1, 17, 42, 100, 128, 129, 1000, 4096, 4097, 100000 };
	int		ko = 0;

	for (unsigned i = 0; i < sizeof(sizes) / sizeof(*sizes); i++)
	{
		void *p = malloc(sizes[i]);
		if (!p)
		{
			printf("KO  malloc(%zu) a retourne NULL\n", sizes[i]);
			ko++;
			continue ;
		}
		if ((uintptr_t)p % 16)
		{
			printf("KO  malloc(%zu) = %p  (mod16 = %lu)\n",
				sizes[i], p, (unsigned long)((uintptr_t)p % 16));
			ko++;
		}
		free(p);
	}
	printf(ko ? "=> ECHEC alignement\n" : "=> OK alignement\n");
	return (ko != 0);
}