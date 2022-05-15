gcc -c -g ./mem-ordering.c 
gcc ./mem-ordering.c -pthread -o ./mem-ordering.out 
echo nofence
./mem-ordering.out | head -n 100000 |sort | uniq -c

gcc -D __DEBUG_MFENCE ./mem-ordering.c -pthread -o ./mem-ordering.out
echo mfence 读写串行化
./mem-ordering.out | head -n 100000 |sort | uniq -c

gcc -D __DEBUG_LFENCE ./mem-ordering.c -pthread -o ./mem-ordering.out
echo lfence 读串行化
./mem-ordering.out | head -n 100000 |sort | uniq -c

gcc -D __DEBUG_SFENCE ./mem-ordering.c -pthread -o ./mem-ordering.out
echo sfence 写串行化
./mem-ordering.out | head -n 100000 |sort | uniq -c

gcc -D __DEBUG_COMPLIER_FENCE ./mem-ordering.c -pthread -o ./mem-ordering.out
echo volatile 禁用编译器指令重排 \(多线程下失效\)
./mem-ordering.out | head -n 100000 |sort | uniq -c


gcc -D __DEBUG_MEMORY_BARRIER -O2  ./mem-ordering.c -pthread -o ./mem-ordering.out
echo __sync_synchronize 禁用编译器指令重排 \(包括隐含条件 : 让编译器也禁止 cpu 硬件的指令重排 \)
./mem-ordering.out | head -n 100000 |sort | uniq -c





