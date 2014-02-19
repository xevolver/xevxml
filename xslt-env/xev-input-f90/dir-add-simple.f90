

SUBROUTINE SAMPLE02

    !$xev dir add(acc, kernels)
    !$xev dir add(acc, update, device(diff, dp, gg, cbar))
    DO I = 1, 15
        !$xev dir add(loop, vector(54))
        DO J = 1, 25
            SUM = SUM + 5
        END DO
    END DO
    !$xev dir add(acc, end, kernels)
    RETURN
END
