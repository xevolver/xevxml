
SUBROUTINE TEST04( M,N )
!$xev array1to2 type(A,M,N)
    INTEGER  idx
    INTEGER A(M*N)
    INTEGER, DIMENSION(M,N) :: B

!$acc kernels loop

!$xev array1to2_varref start(A,i,j)
    do i = 1, M
        do j = 1, N
            !$xev statement remove
            idx = ((i-1)*M)+j

            A(idx) = B(i,j)
        end do
    end do
!$xev end array1to2_varref(A)

    RETURN
END

