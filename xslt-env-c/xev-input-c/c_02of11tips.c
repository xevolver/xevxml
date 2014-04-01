void TEST02()
{
    int found,B[300];
    int i;
    int A[300];

#pragma xev dir append(loop)
#pragma acc kernels

#pragma xev statement remove
    i = 0;
#pragma xev scalar2array1-varref start(found,300,i)
#pragma xev while2for replace(i,0,300)
    do
    {
#pragma xev statement remove
        i = i + 1;
        if (A[i] == 102)
        {
           found = i;
        }
    } while ( found==0 );
#pragma xev end scalar2array1-varref(found,300,i)  
#pragma acc end kernels
    return;
}
