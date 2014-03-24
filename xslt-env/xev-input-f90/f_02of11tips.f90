
SUBROUTINE TEST02()
    INTEGER found
    INTEGER i
    INTEGER, DIMENSION(300) :: A

    !$xev dir append(loop)
    !$acc kernels

    !$xev statement-del ptn-001
    i = 0
    !$xev scalar2array1-varref start(found,N,i)
    !$xev while2do replace(I,1,N)
    do  while ( .not. found==0)
    !$xev statement-del ptn-001
        i = i + 1
        if (A(i) .eq. 102) then
           found = i
        end if
    end do
    !$xev end scalar2array1-varref(found,N,i)
    found = 10
    !$acc end kernels
    
    !print *,'Found at',maxval(found)
    return
END

