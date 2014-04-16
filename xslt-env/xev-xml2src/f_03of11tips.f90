SUBROUTINE TEST03(M,N)
INTEGER, DIMENSION(M,N) :: A
!pragma acc kernels loop copy(A) 
DO i = 1, M
DO j = i, N
A(i,j) = i + j
END DO
END DO
RETURN
END SUBROUTINE 

