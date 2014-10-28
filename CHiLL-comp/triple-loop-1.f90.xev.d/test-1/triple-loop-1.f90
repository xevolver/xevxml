PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
DO k = 2, n - 1
DO j = 2, n - 1
DO i = 2, n - 1
B(i,j,k) = A(i,j,k)
END DO
END DO
END DO
END PROGRAM 

