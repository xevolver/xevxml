PROGRAM triple_loop_1

    INTEGER, PARAMETER :: n = 139

    REAL(kind=8) :: A(n,n,n), B(n,n,n)

    !$xev loop_tag
    DO k = 1, n - 1
        DO j_tile = 1, n - 1, 4
            DO i_tile = 1, n - 1, 4
                DO j = j_tile, min(n - 1, j_tile+4-1)
                    DO i = i_tile, min(n - 1, i_tile+4-1)
                        B(i,j,k) = A(i,j,k)
                    END DO
                END DO
            END DO
        END DO
    END DO

END PROGRAM triple_loop_1

