
program catalog_no9

  integer,parameter :: n=139
  real*8 A(n, n), B(n, n)

  !$xev one_loop_copy flag(n,n)
  do j=2,n-1
     do i=2,n-1
        B(i,j) = A(i,j)
     end do
  end do

end program catalog_no9

