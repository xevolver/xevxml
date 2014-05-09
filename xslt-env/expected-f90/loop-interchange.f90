
SUBROUTINE SAMPLE_INTERCHANGE
  
  DO J = 2, 25
     DO I = 1, 15
        SUM = SUM + 5
     END DO
  END DO
  
  DO K = 3, 35
     DO J = 2, 25
        DO I = 1, 15
           SUM = SUM + 5
        END DO
     END DO
  END DO
  
  
  RETURN
END SUBROUTINE SAMPLE_INTERCHANGE
