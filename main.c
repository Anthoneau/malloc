#include "includes/malloc.h"
#include "includes/libft.h"
//#include <stdio.h>

#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int main(void) {

	size_t size = 17;
	ft_printf("pagesize : %d\n", pagesize);
	char *ptr = (char *)malloc(size);
	if (ptr) write(1, "pointer ok\n", 12);
	else return 1;
	memset(ptr, 'a', size);
	//sleep(1);
	int i = 0;
	while (i < size) {
		if (ptr[i])
			ft_printf("%c", ptr[i]);
		i++;
	}
	write(1, "\n", 1);

	size = 10;
	char *ptr2 = (char *)malloc(size);
	if (ptr) write(1, "pointer ok\n", 12);
	else return 1;
	memset(ptr, 'a', size);
	//sleep(1);
	i = 0;
	while (i < size) {
		if (ptr[i])
			ft_printf("%c", ptr[i]);
		i++;
	}
	write(1, "\n", 1);

	//int i = 0;
	//while (1) {
	//	void *ptr = malloc(100);
	//	if (!ptr)
	//		break ;
	//	ft_printf("%d\t", i);
	//	i++;
	//}

	//int psize = sysconf(_SC_PAGE_SIZE);
	//int pflag = PROT_READ | PROT_WRITE;
	//int type = MAP_PRIVATE | MAP_ANONYMOUS;
	//uint64_t maxsize = 0x100000000;
	//void *base = mmap(NULL, maxsize, pflag, type, -1, 0);
	//munmap(base, maxsize);    

	//char* ptr1 = mmap(base, psize * 10, pflag, type | MAP_FIXED, -1, 0);
	//void* ptr2 = mmap(base + psize * 10, psize, pflag, type | MAP_FIXED, -1, 0);

	//printf("sizeof ptr1 : %d\naddress : %p\n", sizeof(ptr1), &ptr1);
	//printf("sizeof ptr2 : %d\naddress : %p\n", sizeof(ptr2), &ptr2);
	//printf("psize : %d\n", psize);

	//munmap(ptr1, psize*10);
	//munmap(ptr2, psize);

	//int i = 0;
	//while (1) {
	//	ptr1[i] = 'a';
	//	printf("%d ", i);
	//	i++;
	//}

	//write(1, "\n", 1);
	//char *ptr = (char *)malloc(12);
	////write(1, "salut\n", 7);
	////printf("sysconf dans le main : %ld\n", sysconf(10));
	//if (!ptr){
	//	write(1, "pointeur nul\n", 14);
	//	return 1;
	//}
	//write(1, "pointeur non nul\n", 18);
	//char c = '0';
	//for (int i = 0; i < 4096; i++){
	//	if (*ptr)
	//		ft_printf("yo\n");
	//	if (ptr[i]){
	//		if (c <= '9') {
	//			ptr[i] = c;
	//			c++;
	//		}
	//		else {
	//			ptr[i] = '\n';
	//			c = '0';
	//		}
	//	}
	//	else {
	//		ft_printf("%d n'existe pas\n", i);
	//		break ;
	//	}
	//}
	//write(1, "\n", 1);
	return 0;
}
