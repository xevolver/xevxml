SUBROUTINE TEST10(m,n)
REAL :: a(1:100,1:100), b(1:100,1:100)
REAL, DIMENSION(1:100) :: w
REAL :: change, tolerance
INTEGER :: i, j, iters
change = tolerance + 1
iters = 0
!pragma acc data copy(a(1:n 1:m 
dowhile(change>tolerance
!pragma acc kernels 
!pragma xev fortran-do copy-001(a(i,j) = b(i,j))
DO j = 2, n - 1
DO i = 2, m - 1
b(i,j) = 0.250000 * w(i) * (a(i - 1,j) + a(i,j - 1) + a(i + 1,j) + a(i,j + 1)) + (1.00000 - w(i)) * a(i,j)
END DO
END DO
DO j = 2, n - 1
DO i = 2, m - 1
a(i,j) = b(i,j)
END DO
END DO
!pragma acc end kernels 
enddo
!pragma acc end data 
RETURN
END SUBROUTINE 

