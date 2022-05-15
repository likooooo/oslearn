# lesson3
在多处理器 ( 多线程 ) 背景下
## keyword
- 最终一致性
- 提前优化
- cpu 木桶效应
- 内存宽松模型
- μop
## 多处理器下为何会丧失原子性
## 内存乱序
- 硬件层面 : cache miss
```
	load // cache miss
	write
	load // try load again

	如果 load 发生 cache miss, 会继续执行 write 后续再重新 load, 最终看起来是 先 write 后 load .
	解决方法 : 通过 mfence 保证r-w / w-r 两条指令的先后顺序与汇编顺序一致
	lfence : read1-read2 保证两条读指令一致
	sfence : write1-write2 保证两条写指令顺序一致
```
- [编译器指令重排](https://blog.csdn.net/Yooneep/article/details/80119104)
	解决方法 : 
	1. volilate, 多线程下任然无效.
	2. __sync_synchronize, 包括告诉编译器隐含含义 : 既不在编译期进行重排, 也禁止硬件内存乱序.
```
在 gcc -O2 的情况下:
---------------------------------------
volatile int a,b;
void Func()
{
    b = a + 1;
    a = 0;
}
----------------------------------------
Func():
        mov     eax, DWORD PTR a[rip]
        add     eax, 1
        mov     DWORD PTR b[rip], eax
        mov     DWORD PTR a[rip], 0
        ret
------------------------------------------
int a,b;
void Func()
{
    b = a + 1;
    a = 0;
}
------------------------------------------
Func():
        mov     eax, DWORD PTR a[rip]
        mov     DWORD PTR a[rip], 0
        add     eax, 1
        mov     DWORD PTR b[rip], eax
        ret
```
## cpu 本质是
1. cpu 会一次加载多条汇编创建多条 μop 放到 有向无环图, 在电路中并行执行 

