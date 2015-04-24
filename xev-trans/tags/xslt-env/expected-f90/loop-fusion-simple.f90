

SUBROUTINE SAMPLE02

    DO NK=0,NFRAC*(NNODE-1)-1
        n = nk/(nnode-1)+1
        k = mod(nk,nnode-1)+2
        SUM = SUM + 5
    END DO

    RETURN
END
