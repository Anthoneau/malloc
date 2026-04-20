#include "includes/malloc.h"
#include "includes/libft.h"

#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

//#include <stdio.h>
//#include <stdlib.h>

void test_fragmentation(void) {}

void test_function(t_type type) {
	size_t size;

	switch (type)
	{
	case TINY:
		size = 128;
		break ;
	case SMALL:
		size = 1024;
		break ;
	case LARGE:
		size = 2000;
		break ;
	default:
		break ;
	}

	int i = 0;
	char *str;
	void *ptr[200];
	while (i < 200) {
		str = malloc(size);
		ft_memset(str, 48, size);
		ptr[i] = str;
		i++;
	}
	show_alloc_mem();
	// ft_putchar_fd('\n', 1);
	// ft_putstr_fd("str: ", 1);
	// ft_putstr_fd(str, 1);
	// ft_putchar_fd('\n', 1);
	ft_putchar_fd('\n', 1);
	i = 0;
	while (i < 200) {
		free(ptr[i]);
		i++;
	}
	show_alloc_mem();
}

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

void realloc_test() {
	char *ptr = malloc(15);
	ft_strlcpy(ptr, "aaaaaaaaaaaaaaa", 15);
	show_alloc_mem();
	ptr = realloc(ptr, 10);
	show_alloc_mem();
	ptr = realloc(ptr, 120);
	show_alloc_mem();
}

int main(void) {
	//printf("size of t_chunk : %lu\n", sizeof(t_chunk));
	//printf("size of t_zone : %lu\n", sizeof(t_zone));
	// simpletest();
	// test_function(TINY);
	// test_function(SMALL);
	// test_function(LARGE);
	realloc_test();
	return 0;
}
