
program catalog_no9

  integer,parameter :: n=139

  real*8 A(n, n, n), B(n, n, n)

  !$xev one-loop-copy flag(l,m,n)
  do k=2,n-1
     do j=2,n-1
        do i=2,n-1
           B(i,j,k) = A(i,j,k)
        end do
     end do
  end do

  
end program catalog_no9

