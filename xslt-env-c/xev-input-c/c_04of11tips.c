void TEST04( int M, int N )
{
#pragma xev array1to2 type(A,25,25)
    int  idx;
    int  A[625];
    int  B[25][25];
    int i,j;

#pragma acc kernels loop
#pragma xev array1to2_varref start(A,i,j)
    for( i=0; i<M; i++ )
    {
        for( j=0; j<N; j++ )
        {
#pragma xev statement remove
            idx = ((i-1)*M)+j;

            A[idx] = B[i][j];
        }
    }

#pragma xev end array1to2_varref(A)
    return;
}

