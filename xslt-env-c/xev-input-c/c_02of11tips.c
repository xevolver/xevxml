void TEST02()
{
#pragma xev scalar2array1 type(found,300)
#pragma xev scalar2array1 varref(found,i)
    int found;
    int i;
    int A[300];

#pragma xev dir append(loop)
/*
#pragma acc kernels
*/

#pragma xev statement-del ptn-001
    i = 0;
#pragma xev while2do replace(I,1,300)
    do
    {
        i = i + 1;
        if (A[i] == 102)
        {
           found = i;
        }
    } while ( found==0 );
/*
#pragma acc end region
*/
    return;
}
