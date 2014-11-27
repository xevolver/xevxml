SUBROUTINE NT_OPT
    integer IS(10), IT(10)

    DO  M=1,MF
        DO  K=1,KF
            DO  J=1,JF
                DO 200 I=1,inum
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
                    SUM = SUM + 4
200             CONTINUE
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
                        SUM = SUM + 5
                    END DO
                END DO
            END DO
        END DO



        RETURN
    END SUBROUTINE
