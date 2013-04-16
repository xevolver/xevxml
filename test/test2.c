#include <stdio.h>


void saxy(float* aa, float* bb, int size, float alpha)
{
  int i;

  for(i=0;i<size;i++)
    aa[i] = alpha*bb[i] + aa[i];
  return;
}

int main(int argc,char** argv)
{
  int i;
  float a[100],b[100];
#pragma xev hoge
  for(i=0;i<100;i++){
    a[i]=0.0f;
    b[i]=(float)i/100;
  }
  saxpy(a,b,100,0.5);
 
  return 0;
}

