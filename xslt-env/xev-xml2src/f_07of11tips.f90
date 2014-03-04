SUBROUTINE TEST07()
parameter(N = 1024,M = 512) 
REAL :: A(M,N), B(M,N)
!pragma acc kernels loop copyout(A 
DO i = 1, M
DO j = 1, N
A(i,j) = B(i,j + 1)
END DO
END DO
RETURN
END SUBROUTINE 

