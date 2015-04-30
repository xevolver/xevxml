PROGRAM triple_loop_1

    INTEGER, PARAMETER :: n = 139

    REAL(kind=8) :: A(n,n,n), B(n,n,n)

    DO k = 1, n - 1
        DO j = 1, n - 1
            DO i_sm = 1, n - 1, 4
                DO i = i_sm, min(n - 1, i_sm+4-1)
                    B(i,j,k) = A(i,j,k)
                END DO
            END DO
        END DO
    END DO

END PROGRAM triple_loop_1

