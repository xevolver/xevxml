#define N 100

static const volatile int i=N;

int func(int a,double b,int c)
{
  float j=0.0f;  
#pragma xev unroll 2
  for(a=0;a<N;a++)
    c=c+3;

  return 0;
}



