SUBROUTINE SAMPLE02()
!pragma acc kernels 
!pragma acc update device(diff dp gg cbar 
DO I = 1, 15
!pragma loop vector(54 
DO J = 1, 25
SUM = SUM + 5
END DO
END DO
!pragma acc end kernels 
RETURN
END SUBROUTINE 

