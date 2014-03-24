void TEST10( int m, int n )
{
    float a[100][100],b[100][100];
    float w[100];
    float change,tolerance;
    int   i,j,iters;


    change = tolerance + 1;
    iters = 0;
    do
    {
        iters = iters + 1;
        change = 0;

#pragma xev dir add(acc,data,copy(a(1:n,1:m)),create(b(2:n-1,2:m-1)),copyin(w(2:n-1)))


#pragma xev dir replace(acc,kernels)
/*
#pragma acc kernels loop
*/
        for( i=1; i<n-1;i++ )
        {
            for( j=1; j<m-1; j++ )
            {
                b[i][j] = 0.25*w[i]*(a[i-1][j]+a[i][j-1]+ 
                                   a[i+1][j]+a[i][j+1])
                        +(1.0-w[i])*a[i][j];
            }
        }
        for( i=1; i<n-1;i++ )
        {
            for( j=1; j<m-1; j++ )
            {
                a[i][j] = b[i][j];
            }
        }
#pragma xev dir add(acc,end,kernels)

    }while ( change > tolerance );

    return;
}
