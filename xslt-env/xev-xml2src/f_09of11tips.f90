SUBROUTINE TEST09(m,n)
REAL :: a(1:100,1:100), newa(1:100,1:100)
REAL :: w0, w1, w2, change, tolerance
INTEGER :: i, j, iters
change = tolerance + 1
iters = 0
DO WHILE (change > tolerance)
iters = iters + 1
change = 0
!pragma acc kernels copy(a 
!$acc loop reduction(max:change)
DO j = 2, n - 1
DO i = 2, m - 1
newa(i,j) = w0 * a(i,j) + w1 * (a(i - 1,j) + a(i,j - 1) + a(i + 1,j) + a(i,j + 1)) + w2 * (a(i - 1,j - 1) + a(i - 1,j + 1) + a(i + 1,j - 1) + a(i + 1,j + 1))
change = max(change,abs(newa(i,j) - a(i,j)))
END DO
END DO
a(2:(m - 1),2:(n - 1)) = newa(2:(m - 1),2:(n - 1))
!$acc end kernels
RETURN
END SUBROUTINE 

