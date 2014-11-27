

SUBROUTINE SAMPLE02

    !$xev dir replace(acc, parallel)
    !$acc kernels
    DO I = 1, 15
        DO J = 1, 25
            SUM = SUM + 5
        END DO
    END DO


    RETURN
END
