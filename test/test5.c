#define N 100

static const volatile int i=N;

int func(int a,double b,int c)
{
  float j=0.0f;  
#pragma xev unroll
  for(a=0;a<N;a++)
    j=j+0.5f;

  return 0;
}



