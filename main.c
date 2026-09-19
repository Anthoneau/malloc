#include "includes/malloc.h"
#include "includes/libft.h"
#include <stdio.h>
#include <stdint.h>

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
		size = 40000;
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
	ft_putchar_fd('\n', 1);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	//ft_putstr_fd("str: ", 1);
	//ft_putstr_fd(str, 1);
	//ft_putchar_fd('\n', 1);
	//ft_putchar_fd('\n', 1);
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
	ft_strlcpy(test, "salut je suis une pomme", 24);

	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	free(test);

	show_alloc_mem();
}

void realloc_test() {
	ft_putstr_fd("1 : malloc 10\n", 1);
	char *p = malloc(10);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("1 : 10 devient 20\n", 1);
	p = realloc(p, 20);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("1 : 20 devient 5\n", 1);
	p = realloc(p, 5);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("2 : malloc 10\n", 1);
	char *a = malloc(10);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("3 : malloc 10\n", 1);
	char *b = malloc(10);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("3 : free\n", 1);
	free(b);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("2 : 10 devient 15\n", 1);
	a = realloc(a, 15);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("3 : malloc 10\n", 1);
	p = realloc(NULL, 10);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	
	ft_putstr_fd("3 : 10 devient 5000\n", 1);
	p = realloc(p, 5000);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("3: free\n", 1);
	p = realloc(p, 0);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
}

void defragmentation_test() {
	ft_putstr_fd("1 : malloc 10\n", 1);
	char *p = malloc(10);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("2 : malloc 100\n", 1);
	char *a = malloc(100);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("3 : malloc 10\n", 1);
	char *b = malloc(10);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("2 : free\n", 1);
	free(a);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("2 : malloc 15\n", 1);
	a = malloc(15);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("4 : malloc 15\n", 1);
	char *c = malloc(15);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
}

typedef struct s_thread_test {
	int index;
	pthread_mutex_t *write_mutex;
}	t_thtest;

void *multi_malloc(void *arg) {
	t_thtest *thtest = (t_thtest *)arg;
	char *chars_arr[10];
	char *char_id = ft_itoa(thtest->index);
	int id_len = ft_strlen(char_id);
	char *test_str = " test";
	int test_str_len = ft_strlen(test_str);

	int iteration_max = 1;

	for (int i = 0; i < iteration_max; i++) {
		chars_arr[i] = (char *)malloc(id_len + test_str_len + 1);
		// ft_strlcat(chars_arr[i], char_id, id_len + test_str_len + 1);
		ft_strlcpy(chars_arr[i], char_id, id_len + test_str_len + 1);
		ft_strlcat(chars_arr[i], test_str, id_len + test_str_len + 1);
	}
	for (int i = 0; i < iteration_max; i++) {
		pthread_mutex_lock(thtest->write_mutex);
		ft_putendl_fd(chars_arr[i], 1);
		pthread_mutex_unlock(thtest->write_mutex);
		// free(chars_arr[i]);
		// pthread_mutex_lock(write_mutex);
		// ft_putendl_fd("free chars_arr[i]", 1);
		// pthread_mutex_unlock(write_mutex);
	}
	// pthread_mutex_lock(write_mutex);
	// ft_putendl_fd("un peu avant la fin", 1);
	// pthread_mutex_unlock(write_mutex);
	// free(char_id);
	pthread_mutex_lock(thtest->write_mutex);
	ft_putstr_fd("fin ", 1);
	ft_putnbr_fd(thtest->index, 1);
	ft_putchar_fd('\n', 1);
	pthread_mutex_unlock(thtest->write_mutex);
	return NULL;
}

void multi_threaded() {
	pthread_mutex_t write_mutex;
	pthread_mutex_init(&write_mutex, NULL);
	int th_num = 5;
	t_thtest arr[th_num];
	pthread_t threads[th_num];
	for (int i = 0; i < th_num; i++) {
		arr[i].index = i;
		arr[i].write_mutex = &write_mutex;
		int create_status = pthread_create(&threads[i], NULL, &multi_malloc, &arr[i]);
		pthread_mutex_lock(&write_mutex);
		if (create_status== 0) {
			ft_putnbr_fd(i, 1);
			ft_putendl_fd(" ok", 1);
		}
		else {
			ft_putnbr_fd(i, 1);
			ft_putendl_fd(" pas ok", 1);
		}
		pthread_mutex_unlock(&write_mutex);
	}

	for (int i = 0; i < th_num; i++) {
		int join_status = pthread_join(threads[i], NULL);
		pthread_mutex_lock(&write_mutex);
		if (join_status == 0) {
			ft_putnbr_fd(i, 1);
			ft_putendl_fd(" join", 1);
		}
		else {
			ft_putnbr_fd(i, 1);
			ft_putendl_fd(" probleme join", 1);
		}
		pthread_mutex_unlock(&write_mutex);
	}
	pthread_mutex_lock(&write_mutex);
	ft_putendl_fd("\nfin de boucle multithreaded", 1);
	pthread_mutex_unlock(&write_mutex);
	int mutex_status = pthread_mutex_destroy(&write_mutex);
	ft_putstr_fd("mutex_status : ", 1);
	ft_putnbr_fd(mutex_status, 1);
	ft_putchar_fd('\n', 1);
	ft_putendl_fd("mutex detruit", 1);
}

void aligned_test() {
	int it = 4;
	char *tiny[it];
	char *small[it];
	char *large[it];

	ft_putendl_fd("tiny malloc", 1);
	for (int i = 0; i < it; i++)
	{
		tiny[it] = malloc(20);
		printf("%p mod16=%lu\n", tiny[it], (uintptr_t)tiny[it] % 16);
	}

	ft_putendl_fd("small malloc", 1);
	for (int i = 0; i < it; i++)
	{
		small[it] = malloc(200);
		printf("%p mod16=%lu\n", small[it], (uintptr_t)small[it] % 16);
	}

	ft_putendl_fd("large malloc", 1);
	for (int i = 0; i < it; i++)
	{
		large[it] = malloc(100000);
		printf("%p mod16=%lu\n", large[it], (uintptr_t)large[it] % 16);
	}

	show_alloc_mem();

	for (int i = 0; i < it; i++)
	{
		if (small[it])
			free(small[it]);
	}
	for (int i = 0; i < it; i++)
	{
		if (tiny[it])
			free(tiny[it]);
	}
	for (int i = 0; i < it; i++)
	{
		if (large[it])
			free(large[it]);
	}
}

void invalid_pointer() {
	char *addr = malloc(16);
	free(NULL);
	free(addr + 5);
	if (realloc(addr + 5, 10) == NULL)
		write(1, "Bonjour\n", 8);
}

int main(void) {
	//ft_printf("size of t_chunk : %d\n", sizeof(t_chunk));
	//printf("size of t_zone : %lu\n", sizeof(t_zone));
	//printf("size of t_zone : %lu\n", sizeof(t_zone) % 16);
	simpletest();
	test_function(TINY);
	test_function(SMALL);
	test_function(LARGE);
	realloc_test();
	defragmentation_test();
	multi_threaded();
	//multi_malloc(NULL);
	aligned_test();
	invalid_pointer();

	//void *ptr = malloc((size_t)1 << 46);

	//void *ptr = malloc((size_t)-1);
	//show_alloc_mem();
	//printf("%p\n", ptr);

	//char *a = malloc(10);
	//realloc(a, (size_t)1 << 46);
	return 0;
}
