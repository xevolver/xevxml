PROGRAM catalog_no9
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n), B(n,n)
INTEGER, DIMENSION(n,n) :: xev_flg
DO j = 1, n
DO i = 1, n
xev_flg(i,j) = 0
END DO
END DO
DO j = 2, n - 1
DO i = 2, n - 1
xev_flg(i,j) = 1
END DO
END DO
DO i = 1, n * n
IF (xev_ifg(i,1) .EQ. 1) THEN
B(i,1) = A(i,1)
END IF
END DO
END PROGRAM 

