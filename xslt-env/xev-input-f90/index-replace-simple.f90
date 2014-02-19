

SUBROUTINE SAMPLE02
    double precision:: SOCO(30,30,30)
    double precision SUM

    DO I = 1, 10
        DO J = 1, 20
            DO K = 1, 25
                !$xev index replace(I, J)
                SOCO(I,J,K) = SUM + 3.0
            END DO
        END DO
    END DO

    RETURN
END
