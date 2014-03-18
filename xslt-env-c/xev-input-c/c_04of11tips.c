void TEST04( int M, int N )
{
#pragma xev array1to2 type(A,25,25)
    int  idx;
    int  A[625];
    int  B[25][25];
    int i,j;


    #pragma acc kernels loop

#pragma xev array1to2 varref(A,i,j)
    for( i=0; i<M; i++ )
    {
        for( j=0; j<N; j++ )
        {
            #pragma xev statement-del ptn-001
            idx = ((i-1)*M)+j;

            A[idx] = B[i][j];
        }
    }

    return;
}

