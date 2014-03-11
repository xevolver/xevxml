
program catalog_no21

  integer,parameter :: n=139, KB=8, IB=8

  real*8 a(n, n), b(n, n), c(n, n)

  do kk=1,n,KB
  do ii=1,n,IB
  do j=1,n,4
     do k=kk,min(n,kk+KB-1),4
        do i=ii,min(n,ii+IB-1)
           a(i,j) = a(i,j) + b(i,k)*c(k,j) &
                          + b(i,k+1)*c(k+1,j) &
                          + b(i,k+2)*c(k+2,j) &
                          + b(i,k+3)*c(k+3,j)
           a(i,j+1) = a(i,j+1) + b(i,k)*c(k,j+1) &
                          + b(i,k+1)*c(k+1,j+1) &
                          + b(i,k+2)*c(k+2,j+1) &
                          + b(i,k+3)*c(k+3,j+1)
           a(i,j+2) = a(i,j+2) + b(i,k)*c(k,j+2) &
                          + b(i,k+1)*c(k+1,j+2) &
                          + b(i,k+2)*c(k+2,j+2) &
                          + b(i,k+3)*c(k+3,j+2)
           a(i,j+3) = a(i,j+3) + b(i,k)*c(k,j+3) &
                          + b(i,k+1)*c(k+1,j+3) &
                          + b(i,k+2)*c(k+2,j+3) &
                          + b(i,k+3)*c(k+3,j+3)
         end do
      end do
   end do
   end do
   end do
   


end program catalog_no21

