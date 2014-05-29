C test for remove and flatten

      SUBROUTINE SAMPLE03

!$xev remove
      DO L = lstart, lend
        IS = AS(L)
        IE = AE(L)
        DO K = 1, 10
!$xev flatten 1, inum
          DO I = IS, IE
            SUM = SUM + 3
          END DO
        END DO
      END DO

      RETURN
      END
