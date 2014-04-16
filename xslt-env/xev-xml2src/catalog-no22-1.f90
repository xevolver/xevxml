PROGRAM catalog_no22
INTEGER, PARAMETER :: n = 139
REAL :: a1(n,n,n), b1(n,n,n), c1(n,n,n), d1(n,n,n), e1(n,n,n)
DO k = 1, n3
DO j = 1, n2
DO i = 1, n1, 8
a1(i,j,k) = b1(i,j,k) * c1(i,j,k) + d1(i,j,k) * e1(i,j,k)
a1(i + 1,j,k) = b1(i + 1,j,k) * c1(i + 1,j,k) + d1(i + 1,j,k) * e1(i + 1,j,k)
a1(i + 2,j,k) = b1(i + 2,j,k) * c1(i + 2,j,k) + d1(i + 2,j,k) * e1(i + 2,j,k)
a1(i + 3,j,k) = b1(i + 3,j,k) * c1(i + 3,j,k) + d1(i + 3,j,k) * e1(i + 3,j,k)
a1(i + 4,j,k) = b1(i + 4,j,k) * c1(i + 4,j,k) + d1(i + 4,j,k) * e1(i + 4,j,k)
a1(i + 5,j,k) = b1(i + 5,j,k) * c1(i + 5,j,k) + d1(i + 5,j,k) * e1(i + 5,j,k)
a1(i + 6,j,k) = b1(i + 6,j,k) * c1(i + 6,j,k) + d1(i + 6,j,k) * e1(i + 6,j,k)
a1(i + 7,j,k) = b1(i + 7,j,k) * c1(i + 7,j,k) + d1(i + 7,j,k) * e1(i + 7,j,k)
END DO
DO i = n1 - mod(n1,8), n1
a1(i,j,k) = b1(i,j,k) * c1(i,j,k) + d1(i,j,k) * e1(i,j,k)
END DO
END DO
END DO
END PROGRAM 

