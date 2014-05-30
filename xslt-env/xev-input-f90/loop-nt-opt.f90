
SUBROUTINE NT_OPT
    integer AS(10), AE(10)

    !$xev nt_opt param(1, inum)
    DO L = lstart, lend
        IS = AS(L)
        IE = AE(L)
        DO I = IS, IE
            SUM = SUM + 3
        END DO
    END DO

    RETURN
END SUBROUTINE
