PROGRAM catalog_no14
INTEGER, PARAMETER :: n = 139
REAL :: A(n,n,n), B(n,n,n)
!pragma xev loop_unroll step(3)
  !$xev loop_unroll step(3)
DO k = 2, n - 1
!pragma xev loop_unroll step(3)
  !$xev loop_unroll step(3)
DO j = 2, n - 1
!pragma xev loop_unroll step(3)
  !$xev loop_unroll step(3)
DO i = 2, n - 1
B(i,j,k) = A(i,j,k)
END DO
END DO
END DO
END PROGRAM 

