#include <stdio.h>
#ifdef RECURSION_ENABLE
#  include "hanoi-r.c"
#else
#  include <assert.h>
#  include "hanoi-nr.c"
#endif

int main(){
  hanoi(3, 'a', 'b', 'c');
  return 0;
}
