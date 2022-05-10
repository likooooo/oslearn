int main(){
  while(1){
    asm volatile("rdrand %rax");
  }
  return 0;
}
