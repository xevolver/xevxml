PROGRAM triple_loop_1
    INTEGER, PARAMETER :: n = 139
    REAL(kind=8) :: A(n,n,n), B(n,n,n)

    !xev loop_tag
    DO k = 1, n - 1, 4
        DO j = 1, n - 1
            DO i = 1, n - 1
                B(i,j,k) = A(i,j,k)
                B(i,j,k + 1) = A(i,j,k + 1)
                B(i,j,k + 2) = A(i,j,k + 2)
                B(i,j,k + 3) = A(i,j,k + 3)
            END DO
        END DO
    END DO

END PROGRAM triple_loop_1

