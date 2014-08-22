SUBROUTINE NT_OPT
    integer IS(10), IT(10)

    !$acc loop private(L)
    DO  M=1,MF
        !$acc loop gang
        DO  K=1,KF
            !$acc loop gang,vector
            DO  J=1,JF
                !$acc loop vector
                DO I=1,inum
                    !$acc loop seq
                    DO L = lstart, lend
                        IF (I .GE. IS(L) .AND. I .LE. IT(L)) THEN
                            EXIT
                        END IF
                    END DO
                    IF (I.LE.II3.OR.I.GE.IIE)THEN
                        STBC=0.0D0
                    ELSE
                        STBC=1.0D0
                    END IF
                    LOOPBODY = LOOPBODY + 4
                END DO
            END DO
        END DO
    END DO


    DO  M=1,MF
        DO  J=2,JF
            IF (J.LE.JJ2+1.OR.J.GE.JJE) THEN
                STBC=0.0D0
            ELSE
                STBC=1.0D0
            END IF
            DO  K=1,KF
                DO  I=i,inum
                    DO L = lstart, lend
                        IF (I .GE. IS(L) .AND. I .LE. IT(L)) THEN
                            EXIT
                        END IF
                    END DO
                    LOOPBODY = LOOPBODY + 5
                END DO
            END DO
        END DO
    END DO



    RETURN
END SUBROUTINE
