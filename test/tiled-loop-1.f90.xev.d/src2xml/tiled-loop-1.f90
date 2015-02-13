PROGRAM TileLoop
IMPLICIT NONE
INTEGER :: i, j, i1, j1, k
INTEGER, PARAMETER :: N = 3500
REAL :: a(N,N), b(N,N), c(N,N)
REAL :: t1, t2, ta
DO j = 1, N
DO i = 1, N
a(i,j) = i + j
b(i,j) = i * j
c(i,j) = 0.0
END DO
END DO
CALL cpu_time(t1)
DO k = 1, 100
DO j1 = 1, floor(real(N - 1) / 1) * 1 + 1, 1
DO i1 = 1, floor(real(N - 1) / 1) * 1 + 1, 1
DO j = max(1,j1), min(N,j1 + 0)
DO i = max(1,i1), min(N,i1 + 0)
a(i,j) = a(i,j) + b(i,j) * a(i,j)
END DO
END DO
END DO
END DO
END DO
CALL cpu_time(t2)
ta = real(t2 - t1) / 100
    !    print *,  ta
IF (a(1,1) .EQ. 2.53530120E+30) THEN
PRINT *, "OK: ",a(1,1)
ELSE
PRINT *, "NG",a(1,1)
END IF
END PROGRAM TileLoop

