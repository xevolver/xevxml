#include <stdio.h>

int main(int argc,char** argv)
{

#pragma xev test param(10),param2(1,2)
  printf("hello");
#pragma xev test2 param(10)
  return 0;
}
