# gcc -D RECURSION_ENABLE -c -g hanoi.c
gcc -c -g hanoi.c
gcc hanoi.o -o hanoi.out
gdb ./hanoi.out
