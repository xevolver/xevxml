
program catalog_no9

  integer,parameter :: n=139

  real*8 A(n, n), B(n, n)
  integer ifg(n, n)

  do j=1,n
     do i=1,n
        ifg(i,j)=0;
     end do
  end do
  do j=2,n-1
     do i=2,n-1
        ifg(i,j)=1;
     end do
  end do

  do i=1,n*n
     if (ifg(i,1).eq.1) B(i,1) = A(i,1)
  end do

end program catalog_no9

