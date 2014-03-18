void TEST03( int M, int N )
{
    int  A[100][100];
    int i,j;

    #pragma xev dir append(copy(A))
    #pragma acc kernels loop
    for( i=0; i<M; i++ )
    {
        for( j=i; j<N; j++ )
        {
            A[i][j] = i+j;
        }
    }

    return;
}
