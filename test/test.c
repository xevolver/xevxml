#include <stdio.h>


int main(int argc,char** argv)
{
#pragma xev define xevdef "#define hoge 13"
  int i;
  float* a;
  float* b;

#pragma xev cpt regist(a,100*sizeof(float))
  a = (float*)malloc(100*sizeof(float));
  
#pragma xev cpt regist(b,200*sizeof(float))
  b = (float*)malloc(200*sizeof(float));

  init(a,100);
  init(b,200);

#pragma xev cpt dump

  zero(a,100);
  zero(b,200);

#pragma xev cpt restore

  print(a,100);
  print(b,200);

#pragma xev xevdef
  return 0;
}

