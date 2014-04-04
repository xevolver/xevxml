
SUBROUTINE TEST01( M,N )
    INTEGER, DIMENSION(10) :: tmp
    INTEGER, DIMENSION(M,N) :: A

!$acc kernels loop
    DO i = 1, M
    !$xev dir add(acc,loop,private(tmp))
        DO j = 1, N
            DO jj = 1, 10
                tmp(jj) = jj
            END DO
            A(i,j) = SUM(tmp)
        END DO
    END DO

    RETURN
END

