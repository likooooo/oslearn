# gcc -O2  -c -g ./sum.c && gcc ./sum.o -pthread  && gdb ./a.out
gcc -O2  -D __DEBUG_O2 -c -g ./sum.c && gcc ./sum.o -pthread -o ./sum.out
echo gcc O2 :
./sum.out
while (($? == 0))
do
	./sum.out   # 即使99.9999% 的概率执行正确, 但仍然有概率失败
done
echo catched rara bug!
echo gcc O1 :
gcc -O1  -c -g ./sum.c && gcc ./sum.o -pthread -o ./sum.out
./sum.out
echo gcc no optimize :
gcc -c -g ./sum.c && gcc ./sum.o -pthread -o ./sum.out
./sum.out
