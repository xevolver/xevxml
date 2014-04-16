PROGRAM catalog_no9
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
INTEGER, DIMENSION(n,n,n) :: xev_flg
DO k = 1, n
DO j = 1, n
DO i = 1, n
xev_flg(i,j,k) = 0
END DO
END DO
END DO
DO k = 2, n - 1
DO j = 2, n - 1
DO i = 2, n - 1
xev_flg(i,j,k) = 1
END DO
END DO
END DO
DO i = 1, n * n * n
IF (xev_ifg(i,1,1) .EQ. 1) THEN
B(i,1,1) = A(i,1,1)
END IF
END DO
END PROGRAM 

