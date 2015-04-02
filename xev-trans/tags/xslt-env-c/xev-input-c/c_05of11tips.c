#include <stdio.h>
void TEST05( int M, int N )
{
    int  idx;
    int  A[25][25];
    int i,j;

#pragma acc kernels loop

    for( i=0; i<M; i++ )
    {
#pragma xev dir add(acc, loop, private(idx))
        for( j=0; j<N; j++ )
        {
            idx = i+j;
            A[i][j] = idx;
        }
    }
    
    printf( "idx=%d A(1,1)=%d, A(M,N)=%d", idx, A[1][1], A[M][N] );

    return;
}

