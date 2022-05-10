#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
int main(){//_start(){
  // printf("hello world\n");
  // 解决 Cannot access memory at address 0x1
  syscall(SYS_exit, 42);
  return 0;
}
