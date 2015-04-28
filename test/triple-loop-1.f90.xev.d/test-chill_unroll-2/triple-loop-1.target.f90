PROGRAM triple_loop_1
    INTEGER, PARAMETER :: n = 139
    REAL(kind=8) :: A(n,n,n), B(n,n,n)
    !pragma
        !$xev loop_tag1
    DO k = 1, n - 1
        DO j = 1, n - 1
            DO i = 1, n - 1,2
                B(i,j,k) = A(i,j,k)
                B(i+1,j,k) = A(i+1,j,k)
            END DO
        END DO
    END DO
END PROGRAM triple_loop_1
