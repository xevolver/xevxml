PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL(kind=8) :: A(n,n,n), B(n,n,n)
!pragma xev loop_tag
    !$xev loop_tag
DO k = 1, n - 1
DO j = 1, n - 1
IF (DNPOLD(j,m) >= SMALL) THEN
DO i = 1, n - 1
IF (DNPOLD(i,l) >= SMALL) THEN
B(i,j,k) = A(i,j,k)
END IF
END DO
END IF
END DO
END DO
END PROGRAM triple_loop_1

