
SUBROUTINE TEST09(m,n)
    REAL, DIMENSION(1:100,1:100) :: a,newa
    REAL w0,w1,w2,change,tolerance
    INTEGER i,j,iters


      change = tolerance + 1 
      iters = 0
      do while ( change > tolerance )
         iters = iters + 1
         change = 0
!$xev dir append(copy(a),create(newa))
!$acc kernels
!$acc loop reduction(max:change)
         do j = 2, n-1
            do i = 2, m-1
               newa(i,j) = w0 * a(i,j) + &
               w1 * (a(i-1,j) + a(i,j-1) + a(i+1,j) + a(i,j+1) ) + &
               w2 * (a(i-1,j-1) + a(i-1,j+1) + a(i+1,j-1) + a(i+1,j+1) )
              change = max( change, abs( newa(i,j) - a(i,j) ) )
            end do
         end do
         a(2:m-1,2:n-1) = newa(2:m-1,2:n-1)
!$acc end kernels
      end do
 
    RETURN
END

