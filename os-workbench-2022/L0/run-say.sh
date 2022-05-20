gcc -c -o2 -o main.o main.c && gcc -c -o2 -o say.o say.c
# -W1,--verbose 可以看到, 链接比我们想象的复杂得多
gcc -Wl,--verbose main.o say.o -o ./say.out
echo ======================================
./say.out

# http://jyywiki.cn/AbstractMachine/AM_Programs
# ld main.o say.o
ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o main.o say.o -lc /usr/lib/x86_64-linux-gnu/crtn.o
echo ======================================
./a.out
