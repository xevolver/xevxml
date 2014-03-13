
SUBROUTINE TEST10(m,n)
    REAL, DIMENSION(1:100,1:100) :: a,b
    REAL, DIMENSION(1:100) ::w
    REAL change,tolerance
    INTEGER i,j,iters


      change = tolerance + 1
      iters = 0
      do while ( change > tolerance )
         iters = iters + 1
         change = 0

!$xev dir add(acc,data,copy(a(1:n,1:m)),create(b(2:n-1,2:m-1)),copyin(w(2:n-1)))

!$xev dir replace(acc,kernels)
!$acc kernels loop
         do j = 2, n-1
            do i = 2, m-1
              b(i,j) = 0.25*w(i)*(a(i-1,j)+a(i,j-1)+ &              
                                   a(i+1,j)+a(i,j+1)) &
                        +(1.0-w(i))*a(i,j)
            end do
         end do

         do j = 2, n-1
            do i = 2, m-1
              a(i,j) = b(i,j)
            end do
         end do

!$xev dir add(acc,end,kernels)
    end do
 
    RETURN
END

