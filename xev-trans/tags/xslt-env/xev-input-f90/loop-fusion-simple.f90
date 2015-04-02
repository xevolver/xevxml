

SUBROUTINE SAMPLE02

    !$xev loop fusion
    DO N=1,NFRAC
        DO K=1,NNODE
            SUM = SUM + 5
        END DO
    END DO


    RETURN
END
