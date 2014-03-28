PROGRAM catalog_no21
INTEGER, PARAMETER :: n = 139
REAL :: a(n,n), b(n,n), c(n,n)
DO kk = 1, n, 8
DO ii = 1, n, 8
  !$xev blocking block(k,8,i,8)
DO j = 1, n, 4
DO k = kk, min(n,kk + 7), 4
DO i = ii, min(n,ii + 7)
a(i,j) = a(i,j) + b(i,k) * c(k,j) + b(i,k + 1) * c(k + 1,j) + b(i,k + 2) * c(k + 2,j) + b(i,k + 3) * c(k + 3,j)
a(i,j + 1) = a(i,j + 1) + b(i,k) * c(k,j + 1) + b(i,k + 1) * c(k + 1,j + 1) + b(i,k + 2) * c(k + 2,j + 1) + b(i,k + 3) * c(k + 3,j + 1)
a(i,j + 2) = a(i,j + 2) + b(i,k) * c(k,j + 2) + b(i,k + 1) * c(k + 1,j + 2) + b(i,k + 2) * c(k + 2,j + 2) + b(i,k + 3) * c(k + 3,j + 2)
a(i,j + 3) = a(i,j + 3) + b(i,k) * c(k,j + 3) + b(i,k + 1) * c(k + 1,j + 3) + b(i,k + 2) * c(k + 2,j + 3) + b(i,k + 3) * c(k + 3,j + 3)
END DO
END DO
END DO
END DO
END DO
END PROGRAM 

