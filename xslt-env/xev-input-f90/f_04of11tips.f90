
SUBROUTINE TEST04( M,N )
!$xev array1to2 type(A,X,Y)
!$xev array1to2 varref(A,ii,jj)
    INTEGER  idx
    INTEGER A(M*N)
    INTEGER, DIMENSION(M,N) :: B

!$xev array1to2 varref(C,iii,jjj,default)
    !$acc kernels loop

    do i = 1, M
!$xev array1to2 varref(A,i,j)
        A(idx) = B(i,j)
        do j = 1, N
            !$xev statement-del ptn-001
            idx = ((i-1)*M)+j

!$xev array1to2 varref(A,x,y)
            A(idx) = B(i,j)
        end do
!$xev array1to2 varref(A,x,y,end)
            A(idx) = 100
    end do
!$xev array1to2 varref(A,100,300)
            A(idx) = 200

    RETURN
END

