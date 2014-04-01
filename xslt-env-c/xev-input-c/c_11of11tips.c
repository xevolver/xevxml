float x[];
void sub( float *y )
{
    int   i;
#pragma xev dir add(acc,kernels,present_or_copy(y),present_or_copyin(x))
#pragma xev dir remove
#pragma acc declare present(y,x)
#pragma xev dir remove
#pragma acc kernels
    for( i=0; i<100; i++ )
    {
        y[i] = y[i] + x[i];
    }
#pragma acc end kernels

    return;
}

void roo( float *z )
{
#pragma acc data copy(z) copyin(x)
    sub( z );
#pragma acc end data region
}
