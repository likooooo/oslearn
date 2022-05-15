#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int x = 0;

void* Thello(void* id) 
{
  usleep((size_t)(id) * 100000);
  printf("Hello from thread #%c\n", "123456789ABCDEF"[x++]);
}

int main()
{
  	for (size_t i = 0; i < 10; i++)
  	{
		pthread_t tid;
		pthread_create(&tid, NULL, Thello, (void*)i);	
	}
	sleep(2);
	return 0;
}
