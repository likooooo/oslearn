#include "thread.h"
#include <stdatomic.h>

#define FLAG atomic_load(&flag)
#define FLAG_XOR(val) atomic_fetch_xor(&flag, val)
#define WAIT_FOR(cond) while (!(cond)) ;

/* debug */
#ifdef __DEBUG_MFENCE
#	define FENCE "mfence;"
#elif __DEBUG_LFENCE
#	define FENCE "lfence;"
#elif __DEBUG_SFENCE
#	define FENCE "sfence;"
#else
#	define FENCE
#endif
#ifdef __DEBUG_COMPLIER_FENCE
#	define COMPLIER_FENCE volatile
#else
#	define COMPLIER_FENCE
#endif
#ifdef __DEBUG_MEMORY_BARRIER
#	define BARRIER_FENCE __sync_synchronize();
#else
#	define BARRIER_FENCE
#endif

COMPLIER_FENCE int x = 0, y = 0;

atomic_int flag;

 __attribute__((noinline))
void write_x_read_y() {
  int y_val;
#ifndef __DEBUG_MEMORY_BARRIER
  asm volatile(
    "movl $1, %0;" // x = 1
    FENCE
    "movl %2, %1;" // y_val = y
    : "=m"(x), "=r"(y_val) : "m"(y)
  );
#else
  x = 1;
  BARRIER_FENCE;
  y_val = y;
#endif
  printf("%d ", y_val);
}

 __attribute__((noinline))
void write_y_read_x() {
  int x_val;
#ifndef __DEBUG_MEMORY_BARRIER
  asm volatile(
    "movl $1, %0;" // y = 1
    FENCE
    "movl %2, %1;" // x_val = x
    : "=m"(y), "=r"(x_val) : "m"(x)
  );
#else
  y = 1;
  BARRIER_FENCE
  x_val = x;
#endif
  printf("%d ", x_val);
}

void T1(int id) {
  while (1) {
    WAIT_FOR((FLAG & 1));
    write_x_read_y();
    FLAG_XOR(1);
  }
}

void T2() {
  while (1) {
    WAIT_FOR((FLAG & 2));
    write_y_read_x();
    FLAG_XOR(2);
  }
}

void Tsync() {
  while (1) {
    x = y = 0;
    __sync_synchronize(); // full barrier
    usleep(1);            // + delay
    assert(FLAG == 0);
    FLAG_XOR(3);
    // T1 and T2 clear 0/1-bit, respectively
    WAIT_FOR(FLAG == 0);
    printf("\n"); fflush(stdout);
  }
}
/*
 * 状态机
 *		----------
 *		 x  = 0
 * 		 y  = 0
 * 		 pc = 0, 0
 *              -----------
 * 		/	  \
 *	       /           \
 *	   T1 /             \ T2
 * 	     /               \
 * 	    /                 \
 * -----------		      ----------
 *  x  = 1		       x  = 0
 *  y  = 0		       y  = 1
 *  pc = 1, 0		       pc = 0, 1
 * -----------		      -----------
 *           \               /   
 *         T2 \             / T1
 *             \           /
 * 	        \         /
 * 	 	-----------
 *		 x  = 1
 * 		 y  = 1
 * 		 pc = 1, 1
 * 		-----------
 * 0 0  
 * 0 1 只有这一条是符合上述状态迁移结果
 * 1 0 
 * 1 1
 * 
 * 对于上述 4 种情况, 对应数据处理流如下
 * 假定T1 先执行 T2 后执行
 * case 1
 * x	y
 *
 * 0	0	Tsync
 * ------
 * 1	0 	T1 : write_x & read_y
 * 		printf : 0
 * ------
 * 1	1	T2 : write_y & read_x
 * 		printf : 1
 *
 *
 * case 2 : x cache miss
 * x	y
 * 0	0	Tsync
 * ------  
 * 0	0	T1 : write_x // cache miss
 * 0	0	T1 : read_y
 * 		printf : 0
 * ------
 * 0	1 	T2 : write_y 
 * 0	1	T2 : read_x
 * 		printf : 0
 * ------
 * 1	1	T1 : write_x
 *
 *
 * case 3 : x cache 命中, 但 cache 更新至 ram 过慢
 * x	y
 * 0	0	Tsync
 * ------
 * 1	0	T1 : write_x
 * 1	0	T2 : write_y 	// 写入 thread cache,未更新至 ram 
 * 1	0	T2 : read_x
 *		printf : 1
 * 1	0	T1 : read_y
 *		printf 0 
 * 1	1	T2 :		// cache->ram
 *
 * case 5 : x cache 命中且 T1 T2交叉执行
 * x	y
 * 0	0	Tsync
 * ------
 * 0	0	T1 / T2 : write 
 * 1	1	T1 / T2 : cache->ram
 * 1	1	T1 / T2 : read
 * 		printf : 1
 * 		printf : 1
 *
 * 以上 4 种打印情况, 出现概率从高到低依次为
 * 0	0
 * 0	1
 * 1	0
 * 1	1
 *
 * cpu会把汇编进行电路译码,该过程称为 μop  例如:
 * add 1 rax -> Rf[7] += 1
 * add 1 rip -> Rf[8] += 1
 * 
 * cpu 会把多条汇编指令同时翻译到电路中执行, 并且cpu能保证 最终一致性
 * 1. Rf[7] += 1
 * 2. Rf[8] += 1
 * 3. Rf[9] = Rf[7] + Rf[8]
 * 最终一致性可以保证 3 一定在 1 之后执行, 但 1, 2 顺序不确定先后
 *
 * μop包含以下流程:
 * fetch
 * issue
 * execute
 * commit
 *
 * 有向无环图
 *
 * */
int main() {
  create(T1);
  create(T2);
  create(Tsync);
}
