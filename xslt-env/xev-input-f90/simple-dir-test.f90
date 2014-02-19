

SUBROUTINE SAMPLE02

    !$xev test param(10),param2(1,2)
    DO I = 1, 15
        DO J = 1, 25
            SUM = SUM + 5
        END DO
    END DO
    !$xev end test param(10),param2(1,2)

    RETURN
END
