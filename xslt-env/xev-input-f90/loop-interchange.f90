
SUBROUTINE SAMPLE_INTERCHANGE
  
  !$xev loop interchange(1, 2)
  DO I = 1, 15
     DO J = 1, 25
        SUM = SUM + 5
     END DO
  END DO
  
  
  !$xev loop interchange(1, 3)
  DO I = 1, 15
     DO J = 2, 25
        DO K = 3, 35
           SUM = SUM + 5
        END DO
     END DO
  END DO
  
  
  RETURN
END SUBROUTINE SAMPLE_INTERCHANGE
