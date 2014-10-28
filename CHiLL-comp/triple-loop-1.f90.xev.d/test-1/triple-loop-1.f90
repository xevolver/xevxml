PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
DO k = 2, n - 1, 3
DO j = 2, n - 1
DO i = 2, n - 1
B(i,j,k) = A(i,j,k)
B(i,j,k + 1) = A(i,j,k + 1)
B(i,j,k + 2) = A(i,j,k + 2)
END DO
END DO
END DO
END PROGRAM 

