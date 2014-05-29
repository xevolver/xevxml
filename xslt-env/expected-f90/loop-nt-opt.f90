SUBROUTINE NT_OPT

    IS = AS(L)
    IE = AE(L)
    DO I = 1, inum
        DO L = lstart, lend
            IF (I .GE. AS(L) .OR. I .LE. AE(L)) THEN
                EXIT
            END IF
            SUM = SUM + 3
        END DO
    END DO
    RETURN
END SUBROUTINE
