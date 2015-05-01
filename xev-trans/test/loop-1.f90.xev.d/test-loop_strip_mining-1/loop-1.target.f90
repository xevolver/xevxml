PROGRAM triple_loop_1

    INTEGER, PARAMETER :: n = 139

    REAL(kind=8) :: A(n), B(n)

    !$xev loop_tag
    DO i_sm = 1, n, 4
        DO i = i_sm, min(n, i_sm+4-1)
            B(i) = A(i)
        END DO
    END DO

END PROGRAM triple_loop_1

