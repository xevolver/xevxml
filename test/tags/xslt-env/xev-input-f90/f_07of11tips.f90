
SUBROUTINE TEST07()
      parameter(N=1024,M=512)
      real :: A(M,N), B(M,N)


!$xev dir append(copyout(A),copyin(B(0:N,1:M+1)))
!$acc kernels loop
    do i = 1, M
        do j = 1, N
            A(i,j) = B(i,j+1)
        end do
    end do
    
    RETURN
END

