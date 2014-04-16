SUBROUTINE TEST02()
INTEGER :: found, found_tmp(300)
INTEGER :: i
INTEGER, DIMENSION(300) :: A
!pragma acc kernels loop 
DO i = 1, 300
found_tmp(i) = 0
END DO
DO i = 1, 300
IF (A(i) .EQ. 102) THEN
found_tmp(i) = i
END IF
END DO
    !$acc end kernels
    !print *,'Found at',maxval(found)
RETURN
END SUBROUTINE 

