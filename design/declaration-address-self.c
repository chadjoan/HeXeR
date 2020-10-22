#include <stdio.h>

void main()
{
	void *addr = &addr;
	printf("&addr == %p\n", &addr);
	printf("addr  == %p\n", addr);
}
