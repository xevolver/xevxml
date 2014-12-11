PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL :: A(n), B(n)
!pragma xev loop_tag
    !$xev loop_tag
DO i = 1, n
B(i) = A(i)
END DO
END PROGRAM 

