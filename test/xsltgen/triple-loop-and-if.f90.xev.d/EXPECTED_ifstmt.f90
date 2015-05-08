PROGRAM triple_loop_and_if
INTEGER, PARAMETER :: n = 139
REAL(kind=8) :: A(n,n,n), B(n,n,n)
!pragma 
    !$xev loop_tag
DO k = 1, n - 1
DO j = 1, n - 1
DO i = 1, n - 1
B(i,j,k) = A(i,j,k)
END DO
END DO
END DO
!pragma 
!$test
    !$xev loop_tag2
IF (A(1,1,1) > B(1,1,1)) THEN
B(i,j,k) = A(i,j,k)
END IF
END PROGRAM triple_loop_and_if

