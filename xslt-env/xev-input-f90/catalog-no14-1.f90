
program catalog_no14

  integer,parameter :: n=139

  real*8 A(n, n, n), B(n, n, n)

  !$xev loop-unroll step(2)
  do k=2,n-1
     do j=2,n-1
        do i=2,n-1
           B(i,j,k) = A(i,j,k)
        end do
     end do
  end do

end program catalog_no14

