

SUBROUTINE SAMPLE02

    DO IL=0,NNODE*2-1
        i = mod(il,nnode)+1
        l0 = il/nnode+1

        SUM = SUM + 5
    END DO


    RETURN
END
