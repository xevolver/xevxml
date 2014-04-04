#include <stdio.h>

int main(int argc,char** argv)
{

#pragma xev test param(10),param2(1,hoge(1,1),2)
  printf("hello");
#pragma xev test2 param2(1,3,8)
#pragma xev test param2(1,10)
  return 0;
}

