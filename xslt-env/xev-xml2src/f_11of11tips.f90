MODULE glob
REAL, DIMENSION(:), ALLOCATABLE, PUBLIC :: x
CONTAINS
SUBROUTINE sub(y)
REAL, DIMENSION(:), PUBLIC :: y
!pragma acc present_or_copy(y 
DO i = 1, ubound(y,1)
y(i) = y(i) + x(i)
END DO
!$acc end kernels
END SUBROUTINE 

END MODULE glob

SUBROUTINE roo(z,zz)
USE glob
REAL :: z(:), zz
!$acc data copy(z) copyin(x)
sub(z)
sub(zz)
!$acc end data region
END SUBROUTINE 

