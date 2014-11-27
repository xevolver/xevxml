

SUBROUTINE SAMPLE02

    !$xev loop interchange-opt3
    DO J=2,NNODE
        if (DNPOLD(J,M).GE.SMALL) then
            DO I=2,NNODE
                IF (DNPOLD(I,L).GE.SMALL) then
                    SUM = SUM + 5
                end if
            END DO
        end if
    END DO

    RETURN
END
