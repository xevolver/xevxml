
SUBROUTINE SAMPLE_INTERCHANGE
  
    DO J = 2, 25
        DO I = 1, 15
            SUM = SUM + 5
        END DO
    END DO
  
    DO K = 3, 35
        DO J = 2, 25
            DO I = 1, 15
                SUM = SUM + 6
            END DO
        END DO
    END DO
  
    DO I = 1, 15
        DO K = 3, 35
            DO J = 2, 25
                SUM = SUM + 7
            END DO
        END DO
    END DO

    DO I = 1, 15
        DO L = 4, 45
            DO K = 3, 35
                DO J = 2, 25
                    SUM = SUM + 8
                END DO
            END DO
        END DO
    END DO

    DO L = 4, 45
        DO J = 2, 25
            DO K = 3, 35
                DO I = 1, 15
                    SUM = SUM + 9
                END DO
            END DO
        END DO
    END DO

  
    RETURN
END SUBROUTINE SAMPLE_INTERCHANGE
