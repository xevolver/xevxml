

SUBROUTINE SAMPLE02
    !pragma acc parallel
    DO I = 1, 15
        DO J = 1, 25
            SUM = SUM + 5
        END DO
    END DO


    RETURN
END
