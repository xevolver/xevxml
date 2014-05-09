MODULE glob
REAL, DIMENSION(:), ALLOCATABLE :: x
CONTAINS
SUBROUTINE sub(y)
REAL, DIMENSION(:) :: y
!pragma acc kernels present_or_copy(y) present_or_copyin(x) 
DO i = 1, ubound(y,1)
y(i) = y(i) + x(i)
END DO
!$acc end kernels
END SUBROUTINE 

END MODULE glob

SUBROUTINE roo(z)
USE glob
REAL, DIMENSION(:) :: z
!$acc data copy(z) copyin(x)
CALL sub(z)
!$acc end data region
END SUBROUTINE 

