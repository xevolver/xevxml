PROGRAM UnrollTest
IMPLICIT NONE
INTEGER :: loopUpperBound
INTEGER :: i, j, k, m
INTEGER, PARAMETER :: N = 512
REAL :: a(N,N), b(N,N), c(N,N)
REAL :: t1, t2, ta
DO j = 1, N
DO i = 1, N
a(i,j) = i + j
b(i,j) = i * j
END DO
END DO
CALL cpu_time(t1)
DO m = 1, 100
DO i = 1, N
DO j = 1, N
c(i,j) = 0.0
loopUpperBound = N
!pragma xev loop_tag
                !$xev loop_tag
DO k = 1, loopUpperBound, 4
c(i,j) = c(i,j) + b(k,j) * a(i,k)
c(i,j) = c(i,j) + b((k + 1),j) * a(i,(k + 1))
c(i,j) = c(i,j) + b((k + 2),j) * a(i,(k + 2))
c(i,j) = c(i,j) + b((k + 3),j) * a(i,(k + 3))
END DO
END DO
END DO
END DO
CALL cpu_time(t2)
ta = real(t2 - t1) / 100
PRINT *, ta
END PROGRAM UnrollTest

