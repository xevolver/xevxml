

SUBROUTINE SAMPLE02

    DO I=2,NNODE
        IF (DNPOLD(I,L).GE.SMALL) then
            DO J=2,NNODE
                if (DNPOLD(J,M).GE.SMALL) then
                    SUM = SUM + 5
                end if
            END DO
        end if
    END DO

    RETURN
END
