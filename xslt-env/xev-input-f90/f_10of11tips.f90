
SUBROUTINE TEST10(m,n)
    REAL, DIMENSION(1:100,1:100) :: a,b
    REAL, DIMENSION(1:100) ::w
    REAL change,tolerance
    INTEGER i,j,iters


      change = tolerance + 1
      iters = 0
!$xev dir add(acc,data,copy(a(1:n,1:m)) ,reate(b(2:n-1,2:m-1)),copyin(w(2:n-1)))

       !$xev statement-add ptn-000(do,while(change,>,tolerance ) )

!$xev dir replace(acc, kernels)
!$acc kernels loop
      !$xev fortran-do copy-001(a(i,j) = b(i,j))
         do j = 2, n-1
            do i = 2, m-1
              b(i,j) = 0.25*w(i)*(a(i-1,j)+a(i,j-1)+ &              
                                   a(i+1,j)+a(i,j+1)) &
                        +(1.0-w(i))*a(i,j)
            end do
         end do
!$xev dir add(acc, end, kernels)
      !$xev statement-add ptn-000(end,do )
 
!$xev dir add(acc, end, data)
    RETURN
END

