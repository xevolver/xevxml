
SUBROUTINE NT_OPT
    integer IS(10), IT(10)

    !$xev nt_opt200
    DO  M=1,MF
        DO  K=1,KF
            DO  J=1,JF
                DO  L=lstart,lend
                    II1 = IS(L)
                    II2 = II1+1
                    II3 = II2+1
                    IIF = IT(L)
                    IIE = IIF-1
                    IID = IIE-1
                    DO I=II2,IIF
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
    END DO


    !$xev nt_opt2
    DO  L=lstart,lend
        II1 = IS(L)
        II2 = II1+1
        IIF = IT(L)
        IIE = IIF-1
        DO  M=1,MF
            DO  J=2,JF
                IF (J.LE.JJ2+1.OR.J.GE.JJE) THEN
                    STBC=0.0D0
                ELSE
                    STBC=1.0D0
                END IF
                DO  K=1,KF
                    DO I=II1,IIF

                        LOOPBODY = LOOPBODY + 5

                    END DO
                END DO
            END DO
        END DO
    END DO

    RETURN
END SUBROUTINE
