
SUBROUTINE TEST04( M,N )
!$xev array1to2_varref start(A,ii,jj,default)
    INTEGER  idx
    INTEGER A(M*N)
    INTEGER, DIMENSION(M,N) :: B

!$xev array1to2 type(A,X,Y)
    !$acc kernels loop

    do i = 1, M
!$xev array1to2_varref start(A,i,j)
        A(idx) = B(i,j)
        do j = 1, N
            !$xev statement remove
            idx = ((i-1)*M)+j

!$xev array1to2_varref start(A,x,y)
            A(idx) = B(i,j)
        end do
!$xev end array1to2_varref(A)
            A(idx) = 100
    end do

    RETURN
END

