PROGRAM catalog_no14
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
DO k = 2, n - 1, 3
DO j = 2, n - 1, 3
DO i = 2, n - 1, 3
B(i,j,k) = A(i,j,k)
B(i,j,k + 1) = A(i,j,k + 1)
B(i,j,k + 2) = A(i,j,k + 2)
B(i,j + 1,k) = A(i,j + 1,k)
B(i,j + 1,k + 1) = A(i,j + 1,k + 1)
B(i,j + 1,k + 2) = A(i,j + 1,k + 2)
B(i,j + 2,k) = A(i,j + 2,k)
B(i,j + 2,k + 1) = A(i,j + 2,k + 1)
B(i,j + 2,k + 2) = A(i,j + 2,k + 2)
B(i + 1,j,k) = A(i + 1,j,k)
B(i + 1,j,k + 1) = A(i + 1,j,k + 1)
B(i + 1,j,k + 2) = A(i + 1,j,k + 2)
B(i + 1,j + 1,k) = A(i + 1,j + 1,k)
B(i + 1,j + 1,k + 1) = A(i + 1,j + 1,k + 1)
B(i + 1,j + 1,k + 2) = A(i + 1,j + 1,k + 2)
B(i + 1,j + 2,k) = A(i + 1,j + 2,k)
B(i + 1,j + 2,k + 1) = A(i + 1,j + 2,k + 1)
B(i + 1,j + 2,k + 2) = A(i + 1,j + 2,k + 2)
B(i + 2,j,k) = A(i + 2,j,k)
B(i + 2,j,k + 1) = A(i + 2,j,k + 1)
B(i + 2,j,k + 2) = A(i + 2,j,k + 2)
B(i + 2,j + 1,k) = A(i + 2,j + 1,k)
B(i + 2,j + 1,k + 1) = A(i + 2,j + 1,k + 1)
B(i + 2,j + 1,k + 2) = A(i + 2,j + 1,k + 2)
B(i + 2,j + 2,k) = A(i + 2,j + 2,k)
B(i + 2,j + 2,k + 1) = A(i + 2,j + 2,k + 1)
B(i + 2,j + 2,k + 2) = A(i + 2,j + 2,k + 2)
END DO
DO i = n - 1 - mod(n - 1,3), n - 1
B(i,j,k) = A(i,j,k)
B(i,j,k + 1) = A(i,j,k + 1)
B(i,j,k + 2) = A(i,j,k + 2)
B(i,j + 1,k) = A(i,j + 1,k)
B(i,j + 1,k + 1) = A(i,j + 1,k + 1)
B(i,j + 1,k + 2) = A(i,j + 1,k + 2)
B(i,j + 2,k) = A(i,j + 2,k)
B(i,j + 2,k + 1) = A(i,j + 2,k + 1)
B(i,j + 2,k + 2) = A(i,j + 2,k + 2)
END DO
END DO
DO j = n - 1 - mod(n - 1,3), n - 1
DO i = 2, n - 1
B(i,j,k) = A(i,j,k)
B(i,j,k + 1) = A(i,j,k + 1)
B(i,j,k + 2) = A(i,j,k + 2)
END DO
END DO
END DO
DO k = n - 1 - mod(n - 1,3), n - 1
DO j = 2, n - 1
DO i = 2, n - 1
B(i,j,k) = A(i,j,k)
END DO
END DO
END DO
END PROGRAM 

