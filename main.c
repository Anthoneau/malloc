#include "includes/malloc.h"
#include "includes/libft.h"

#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

//#include <stdio.h>
//#include <stdlib.h>

void test_fragmentation(void) {}

void test_tiny() {
	int i = 0;
	char *str;
	void *ptr[200];
	while (i < 200) {
		str = malloc(128);
		ft_strlcpy(str, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 128);
		ptr[i] = str;
		i++;
	}
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	i = 0;
	while (i < 200) {
		free(ptr[i]);
		i++;
	}
	show_alloc_mem();
}

void test_small() {}

void test_large() {}

void simpletest() {
	char *test = "test string";
	size_t size = ft_strlen(test) + 1;
	char *str = (char *)malloc(size);
	ft_strlcpy(str, test, size);

	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	char *str2 = (char *)malloc(120);
	
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	free(str);

	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	
	free(str2);

	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	test = malloc(25);
	ft_strlcpy(test, "salut je m'appelle pomme", 25);

	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	free(test);

	show_alloc_mem();
}

int main(void) {
	//simpletest();
	//printf("size of t_chunk : %lu\n", sizeof(t_chunk));
	//printf("size of t_zone : %lu\n", sizeof(t_zone));
	//printf("size of size_t : %lu\n", sizeof(size_t));
	//printf("size of unsigned long : %lu\n", sizeof(unsigned long));
	//printf("size of int : %lu\n", sizeof(int));
	test_tiny();
	return 0;
}
