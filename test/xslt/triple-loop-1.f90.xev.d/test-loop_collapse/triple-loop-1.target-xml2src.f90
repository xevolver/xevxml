PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
DO j = 1, n - 1
DO i = 1, (n - 1) * (n - 1)
B(i,j,1) = A(i,j,1)
END DO
END DO
END PROGRAM 

