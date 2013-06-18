#define N 100

static const volatile int i=N;

int func(int a,double b,int c)
{
  float j=0.0f;  
#pragma xev unroll 2
  for(;;)
    c=c+3;

  return 0;
}



