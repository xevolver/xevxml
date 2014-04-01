void TEST09( int m, int n )
{
    float  a[100][199], newa[100][100];
    float  w0,w1,w2,change,tolerance;
    int    i,j,iters;


    change = tolerance + 1;
    iters = 0;
    do
    {
        iters = iters + 1;
        change = 0;
#pragma xev dir append(copy(a)  create(newa))
#pragma acc kernels
#pragma acc loop reduction(max:change)
        for( i=1; i<n-1; i++ )
        {
            for( j=1; j<m-1; j++ )
            {
               newa[i][j] = w0 * a[i][j] + 
               w1 * (a[i-1][j] + a[i][j-1] + a[i+1][j] + a[i][j+1] ) + 
               w2 * (a[i-1][j-1] + a[i-1][j+1] + a[i+1][j-1] + a[i+1][j+1] );
              change = max( change, abs( newa[i][j] - a[i][j] ) );
            }
        }

        for( i=1; i<n-1;i++ )
        {
            for( j=1; j<m-1; j++ )
            {
                a[i][j] = newa[i][j];
            }
        }
#pragma acc end kernels
    }while ( change > tolerance );
 
}
