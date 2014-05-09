
SUBROUTINE SAMPLE_INTERCHANGE
  
    !$xev loop interchange(1)
    DO I = 1, 15
        DO J = 2, 25
            SUM = SUM + 5
        END DO
    END DO
  
  
    !$xev loop interchange(2)
    DO I = 1, 15
        DO J = 2, 25
            DO K = 3, 35
                SUM = SUM + 6
            END DO
        END DO
    END DO
  
    DO I = 1, 15
        !$xev loop interchange(1)
        DO J = 2, 25
            DO K = 3, 35
                SUM = SUM + 7
            END DO
        END DO
    END DO

    DO I = 1, 15
        !$xev loop interchange(2)
        DO J = 2, 25
            DO K = 3, 35
                DO L = 4, 45
                    SUM = SUM + 8
                END DO
            END DO
        END DO
    END DO

  
    RETURN
END SUBROUTINE SAMPLE_INTERCHANGE
