
SUBROUTINE TEST02()
    INTEGER found
    INTEGER i
    INTEGER, DIMENSION(300) :: A

    !$xev dir append(loop)
    !$acc kernels

    !$xev statement remove
    i = 0
    !$xev scalar2array1_varref start(found,300,i)
    !$xev while2do replace(i,1,300)
    do  while ( .not. found==0)
    !$xev statement remove
        i = i + 1
        if (A(i) .eq. 102) then
           found = i
        end if
    end do
    !$xev end scalar2array1_varref(found)
    !$acc end kernels
    
    !print *,'Found at',maxval(found)
    return
END

