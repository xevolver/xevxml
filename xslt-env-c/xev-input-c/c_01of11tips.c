
void TEST01( int M,int N )
{
    int	i,j,jj;
    int tmp[10];
    int	A[10][10];

#pragma acc kernels loop
/*
#pragma xev dir add(acc,loop,private(tmp))
*/
    for( i=0; i<M; i++ )
    {
        for( j=0; j<N; j++ )
        {
            for( jj=0; jj<10; jj++ )
            {
                tmp[jj] = jj;
            }
            A[i][j]= SUM(tmp);
        }
    }

    return;
}

