#include "includes/malloc.h"
#include "includes/libft.h"

#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

//#include <stdio.h>
//#include <stdlib.h>

void test_fragmentation(void) {}

void test_tiny() {}

void test_small() {}

void test_large() {}

void simpletest() {}

int main(void) {
	ft_printf("pagesize : %d\n", pagesize);

	char *test = "test string";
	size_t size = ft_strlen(test) + 1;
	char *str = (char *)malloc(size);
	ft_strlcpy(str, test, size);
	ft_putendl_fd(str, 1);
	
	show_alloc_mem();
	
	free(str);

	//char *test2 = "test string 2";
	//size = ft_strlen(test2) + 1;
	//char *str2 = (char *)malloc(size);
	//ft_strlcpy(str2, test2, size);
	//ft_putendl_fd(str2, 1);

	//size = 5;
	//int *arr = malloc(size * sizeof(int));
	//int i = 0;
	//while (i < size) {
	//	arr[i] = i + 1;
	//	i++;
	//}

	//i = 0;
	//while (i < size) {
	//	ft_putstr_fd("putnbr : ", 1);
	//	ft_putnbr_fd(arr[i], 1);
	//	ft_putchar_fd('\n', 1);
	//	i++;
	//}

	//void *ptr = malloc(0);

	//free(str);
	//free(ptr);

	//size_t size = 5;
	//int *arr = malloc(size * sizeof(int));
	//int i = 0;
	//while (i < 100001) {
	//	printf("i : %d\n", i);
	//	arr[i] = i + 1;
	//	i++;
	//}
	//i = 0;
	//while (i < 100001) {
	//	printf("arr[%d] : %d\n", i, arr[i]);
	//	i++;
	//}

	//while (1) {
	//	sleep(1);
	//	void *ptr = malloc(10);
	//}
	return 0;
}
