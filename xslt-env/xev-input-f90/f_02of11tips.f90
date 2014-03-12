
SUBROUTINE TEST02()
    !$xev scalar2array1 type(found,N)
    INTEGER found
    INTEGER i
    INTEGER, DIMENSION(300) :: A

    !$xev dir append(loop)
    !$acc kernels

    !$xev statement-del ptn-001
    i = 0
    !$xev scalar2array1 varref(found,i)
    !$xev while2do replace(I,1,N)
    do  while ( .not. found==0)
    !$xev statement-del ptn-001
        i = i + 1
        if (A(i) .eq. 102) then
           found = i
        end if
    end do
    !$xev scalar2array1 varref(found,i,end)
    found = 10
    !$acc end region
    
    !print *,'Found at',maxval(found)
    return
END

