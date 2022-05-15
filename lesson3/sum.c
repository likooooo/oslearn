#include "thread.h"
#include <assert.h>

#define N 10000000
#define PrintSumResult(result) printf(#result " = %lu( should be %d )\n", result, 2 * N);

unsigned long sum1;
void Sum1()
{
	for(size_t i = 0; i < N; i++)
	{
		sum1++;
		//asm volatile("lock add $1, %0" : "+m"(sum1));
	}
}
volatile unsigned long sum2;
void Sum2()
{
	for(size_t i = 0; i < N; i++)
	{
		asm volatile("":::"memory");
		sum2++;
		asm volatile("":::"memory");
	}
}

int main() 
{
	create(Sum1);
	create(Sum1);
	join();
	PrintSumResult(sum1);
#ifdef __DEBUG_O2
	assert(2 *N == sum1);
#endif
	//create(Sum2);
	//create(Sum2);
	//join();
	//PrintSumResult(sum2);
	return sum1;
}
