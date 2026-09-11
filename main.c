#include "includes/malloc.h"
#include "includes/libft.h"

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
	ft_putstr_fd("str: ", 1);
	ft_putstr_fd(str, 1);
	ft_putchar_fd('\n', 1);
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

	ft_putstr_fd("3: free\n", 1);
	p = realloc(p, 0);
	show_alloc_mem();
	ft_putchar_fd('\n', 1);
}

void defragmentation() {
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

void *multi_malloc(void *arg) {
	pthread_mutex_t *write_mutex = (pthread_mutex_t *)arg;
	// pthread_mutex_lock(&write_mutex);
	pthread_t id = pthread_self();
	// ft_putnbr_fd(id, 1);
	// ft_putendl_fd(" pthread self", 1);
	char *char_id = ft_itoa(id);
	// ft_putendl_fd("itoa", 1);
	int id_len = ft_strlen(char_id);
	// ft_putendl_fd("strlen", 1);
	void *ptr[10];
	char *test_str = " test";
	int test_str_len = ft_strlen(test_str);
	// ft_putendl_fd("strlen2", 1);
	int iteration_max = 1;
	// pthread_mutex_unlock(&write_mutex);

	for (int i = 0; i < iteration_max; i++) {
		ptr[i] = (char *)malloc(id_len + test_str_len + 1);
		// ft_strlcat(ptr[i], char_id, id_len + test_str_len + 1);
		ft_strlcpy(ptr[i], char_id, id_len + test_str_len + 1);
		ft_strlcat(ptr[i], test_str, id_len + test_str_len + 1);
	}
	for (int i = 0; i < iteration_max; i++) {
		pthread_mutex_lock(write_mutex);
		ft_putendl_fd(ptr[i], 1);
		pthread_mutex_unlock(write_mutex);
		// free(ptr[i]);
		// pthread_mutex_lock(write_mutex);
		// ft_putendl_fd("free ptr[i]", 1);
		// pthread_mutex_unlock(write_mutex);
	}
	// pthread_mutex_lock(write_mutex);
	// ft_putendl_fd("un peu avant la fin", 1);
	// pthread_mutex_unlock(write_mutex);
	// free(char_id);
	pthread_mutex_lock(write_mutex);
	ft_putendl_fd("thread finito", 1);
	pthread_mutex_unlock(write_mutex);
	return NULL;
}

void multi_threaded() {
	pthread_mutex_t write_mutex;
	pthread_mutex_init(&write_mutex, NULL);
	pthread_t threads[5];
	for (int i = 0; i < 5; i++) {
		int create_status = pthread_create(&threads[i], NULL, &multi_malloc, &write_mutex);
		pthread_mutex_lock(&write_mutex);
		ft_putstr_fd("create_status : ", 1);
		ft_putnbr_fd(create_status, 1);
		ft_putchar_fd('\n', 1);
		pthread_mutex_unlock(&write_mutex);
	}

	for (int i = 0; i < 5; i++) {
		pthread_mutex_lock(&write_mutex);
		ft_putnbr_fd(i, 1);
		ft_putendl_fd(" est join", 1);
		pthread_mutex_unlock(&write_mutex);
		int join_status = pthread_join(threads[i], NULL);
		pthread_mutex_lock(&write_mutex);
		ft_putstr_fd("join_status : ", 1);
		ft_putnbr_fd(join_status, 1);
		ft_putchar_fd('\n', 1);
		pthread_mutex_unlock(&write_mutex);
	}
	pthread_mutex_lock(&write_mutex);
	ft_putendl_fd("apres la boucle de multithreaded", 1);
	pthread_mutex_unlock(&write_mutex);
	int mutex_status = pthread_mutex_destroy(&write_mutex);
	ft_putstr_fd("mutex_status : ", 1);
	ft_putnbr_fd(mutex_status, 1);
	ft_putchar_fd('\n', 1);
	ft_putendl_fd("le mutex a taille", 1);
}

int main(void) {
	// ft_printf("size of t_chunk : %d\n", sizeof(t_chunk));
	// printf("size of t_zone : %lu\n", sizeof(t_zone));
	// simpletest();
	// test_function(TINY);
	// test_function(SMALL);
	// test_function(LARGE);
	// realloc_test();
	// defragmentation();
	multi_threaded();
	// multi_malloc(NULL);
	return 0;
}
