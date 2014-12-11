PROGRAM triple_loop_1
INTEGER, PARAMETER :: n = 139
REAL :: A(n), B(n), C(n), D(n)
DO i = 1, n
B(i) = A(i)
END DO
DO i = 1, n
D(i) = C(i)
END DO
END PROGRAM 

