
SUBROUTINE TEST05( M,N )
    INTEGER  idx
    INTEGER, DIMENSION(M,N) :: A

!$acc kernels loop

    do i = 1, M
!$xev dir add(acc, loop, private(idx))
        do j = 1, N
            idx = i+j
            A(i,j) = idx
        end do
    end do
    
    print *, idx, A(1,1), A(M,N)
    RETURN
END

