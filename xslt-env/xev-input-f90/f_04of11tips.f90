
SUBROUTINE TEST04( M,N )
    INTEGER  idx
    INTEGER A(M*N)
    INTEGER, DIMENSION(M,N) :: B

    !$xev dir replace(acc, kernels, loop)
    !$acc kernels loop

    do i = 1, M
        do j = 1, N
            !$xev statement-del ptn-001
            idx = ((i-1)*M)+j

            !$xev statement-rep ptn-001(A(i,j)=B(i,j))
            A(idx) = B(i,j)
        end do
    end do

    RETURN
END

