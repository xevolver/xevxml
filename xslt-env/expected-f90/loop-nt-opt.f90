SUBROUTINE NT_OPT

    DO I = 1, inum
        DO L = lstart, lend
            IF (I .GE. AS(L) .AND. I .LE. AE(L)) THEN
                EXIT
            END IF
            SUM = SUM + 3
        END DO
    END DO
    RETURN
END SUBROUTINE
