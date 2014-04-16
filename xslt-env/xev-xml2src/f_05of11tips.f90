SUBROUTINE TEST05(M,N)
INTEGER :: idx
INTEGER, DIMENSION(M,N) :: A
!$acc kernels loop
DO i = 1, M
!pragma acc loop private(idx) 
DO j = 1, N
idx = i + j
A(i,j) = idx
END DO
END DO
PRINT *, idx,A(1,1),A(M,N)
RETURN
END SUBROUTINE 

