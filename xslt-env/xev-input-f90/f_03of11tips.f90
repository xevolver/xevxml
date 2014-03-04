
SUBROUTINE TEST03( M,N )
    INTEGER, DIMENSION(M,N) :: A

    !$xev dir replace(acc, kernels, copy(A))
    !$acc kernels loop
    do i = 1, M
        do j = i, N
            A(i,j) = i+j
        end do
    end do

    RETURN
END

