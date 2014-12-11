PROGRAM triple_loop_1
    INTEGER, PARAMETER :: n = 139
    REAL(kind=8) :: A(n,n,n), B(n,n,n)
    DO k = 1, n - 1, 4
        DO j = 1, n - 1
            DO i = 1, n - 1, 2
                B(i,j,k) = A(i,j,k)
                B(i + 1,j,k) = A(i + 1,j,k)
                B(i,j,k + 1) = A(i,j,k + 1)
                B(i + 1,j,k + 1) = A(i + 1,j,k + 1)
                B(i,j,k + 2) = A(i,j,k + 2)
                B(i + 1,j,k + 2) = A(i + 1,j,k + 2)
                B(i,j,k + 3) = A(i,j,k + 3)
                B(i + 1,j,k + 3) = A(i + 1,j,k + 3)
            END DO
        END DO
    END DO
END PROGRAM triple_loop_1

