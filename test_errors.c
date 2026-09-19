#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int	main(void)
{
	int		ko = 0;
	char	*addr = malloc(16);

	free(NULL);
	free((void *)addr + 5);
	if (realloc((void *)addr + 5, 10) != NULL)
	{
		printf("KO  realloc(ptr invalide) devrait retourner NULL\n");
		ko++;
	}
	free(addr);
	free(addr);
	printf("OK  pointeurs invalides\n");

	if (malloc((size_t)-1) != NULL)
	{
		printf("KO  malloc(SIZE_MAX) devrait retourner NULL\n");
		ko++;
	}
	if (malloc((size_t)1 << 46) != NULL)
	{
		printf("KO  malloc(1<<46) devrait retourner NULL\n");
		ko++;
	}
	char *a = malloc(10);
	if (realloc(a, (size_t)1 << 46) != NULL)
	{
		printf("KO  realloc trop gros devrait retourner NULL\n");
		ko++;
	}
	if (!a || *a == 1)
		;
	free(a);
	if (malloc(10) == NULL)
	{
		printf("KO  malloc(10) apres un echec\n");
		ko++;
	}
	printf("OK  tailles invalides\n");
	printf(ko ? "=> ECHEC erreurs\n" : "=> OK erreurs\n");
	return (ko != 0);
}