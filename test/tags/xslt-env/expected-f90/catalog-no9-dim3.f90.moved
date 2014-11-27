
program catalog_no9

  integer,parameter :: n=139

  real*8 A(n, n, n), B(n, n, n)
  integer ifg(n, n, n)

  do k=1,n
     do j=1,n
        do i=1,n
           ifg(i,j,k)=0;
        end do
     end do
  end do
  do k=2,n-1
     do j=2,n-1
        do i=2,n-1
           ifg(i,j,k)=1;
        end do
     end do
  end do
     
  do i=1,n*n*n
     if (ifg(i,1,1).eq.1) B(i,1,1) = A(i,1,1)
  end do

end program catalog_no9

