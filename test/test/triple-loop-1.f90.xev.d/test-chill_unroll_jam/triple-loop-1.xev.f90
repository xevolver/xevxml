PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
DO k = 1, n - 1, 4
DO j = 1, n - 1
DO i = 1, n - 1
B(i,j,k) = A(i,j,k)
B(i,j,k + 1) = A(i,j,k + 1)
B(i,j,k + 2) = A(i,j,k + 2)
B(i,j,k + 3) = A(i,j,k + 3)
END DO
END DO
END DO
END PROGRAM 

