

SUBROUTINE SAMPLE02

    DO I = 1, 15
        DO J = 1, 25
            !$xev var replace(SUM, TASU1)
            !$xev var replace(TASU1, TASU2)
            !$xev var replace(TASU2, TASU3)
            SUM = SUM + 5
        END DO
    END DO


    RETURN
END
