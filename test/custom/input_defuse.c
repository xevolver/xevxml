#include <stdio.h>
int main()
{
  int x = 9;
  #pragma hoge
  if(x>0)
    x = x + 1;
  else
    x = x + 2;
  printf("%d\n",x);
}
