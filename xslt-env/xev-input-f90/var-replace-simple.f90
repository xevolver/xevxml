

SUBROUTINE SAMPLE02

    DO I = 1, 15
        DO J = 1, 25
            !$xev var replace(SUM, TASU)
            SUM = SUM + 5
        END DO
    END DO


    RETURN
END
