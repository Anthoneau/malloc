/* realloc : contenu preserve, cas limites */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(void)
{
	int ko = 0;

	char *a = malloc(10);
	strcpy(a, "abcdefgh");
	a = realloc(a, 5000);
	if (!a || strcmp(a, "abcdefgh"))
		ko += printf("KO  contenu perdu a l'agrandissement\n");
	memset(a, 'B', 4999);
	a[4999] = 0;
	a = realloc(a, 20);
	if (!a || a[0] != 'B')
		ko += printf("KO  contenu perdu a la reduction\n");
	free(a);

	char *b = realloc(NULL, 50);
	if (!b)
		ko += printf("KO  realloc(NULL, 50)\n");
	if (realloc(b, 0) != NULL)
		ko += printf("KO  realloc(ptr, 0) devrait retourner NULL\n");

	char *c = malloc(10);
	strcpy(c, "12345");
	c = realloc(c, 200000);
	if (!c || strcmp(c, "12345"))
		ko += printf("KO  contenu perdu TINY -> LARGE\n");
	free(c);

	printf(ko ? "=> ECHEC realloc\n" : "=> OK realloc\n");
	return (ko != 0);
}
