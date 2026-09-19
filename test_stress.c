#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fill(size_t size, int count)
{
	void **tab = malloc(count * sizeof(void *));
	if (!tab)
		return (printf("KO  tableau\n"), 1);
	for (int i = 0; i < count; i++)
	{
		tab[i] = malloc(size);
		if (!tab[i])
			return (printf("KO  malloc(%zu) n.%d\n", size, i), 1);
		memset(tab[i], 'A', size);
	}
	for (int i = 0; i < count; i++)
		free(tab[i]);
	free(tab);
	printf("OK  %d x malloc(%zu)\n", count, size);
	return (0);
}

int	main(void)
{
	int ko = 0;

	ko += fill(1, 500);
	ko += fill(100, 500);
	ko += fill(128, 500);
	ko += fill(129, 300);
	ko += fill(4096, 300);
	ko += fill(100000, 50);
	for (int i = 0; i < 2000; i++)
	{
		char *p = malloc(64);
		if (!p)
			return (printf("KO  boucle malloc/free\n"), 1);
		free(p);
	}
	printf("OK  2000 x malloc(64)/free\n");
	printf(ko ? "=> ECHEC stress\n" : "=> OK stress\n");
	return (ko != 0);
}