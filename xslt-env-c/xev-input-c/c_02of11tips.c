void TEST02()
{
    int found;
    int i;
    int A[300];

#pragma xev dir append(loop)
#pragma acc kernels

#pragma xev statement remove
    i = 0;
#pragma xev scalar2array1_varref start(found,300,i)
#pragma xev while2for replace(i,0,300)
    while ( found==0 )
    {
#pragma xev statement remove
        i = i + 1;
        if (A[i] == 102)
        {
           found = i;
        }
    }
#pragma xev end scalar2array1_varref(found,300,i)  
#pragma acc end kernels
    return;
}
