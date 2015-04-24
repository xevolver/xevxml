
program catalog_no14

  integer,parameter :: n=139

  real*8 A(n, n, n), B(n, n, n)

  do k=2,n-1,2
     do j=2,n-1
        do i=2,n-1
           B(i,j,k) = A(i,j,k)
           B(i,j,k+1) = A(i,j,k+1)
        end do
     end do
  end do

end program catalog_no14

