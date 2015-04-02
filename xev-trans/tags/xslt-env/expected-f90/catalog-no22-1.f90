
program catalog_no21

  integer,parameter :: n=139

  real*8 a1(n,n,n),b1(n,n,n),c1(n,n,n),d1(n,n,n),e1(n,n,n)

  do k=1,n3
     do j=1,n2
        a1(1,j,k)=b1(1,j,k)*c1(1,j,k)+d1(1,j,k)*e1(1,j,k)
        a1(2,j,k)=b1(2,j,k)*c1(2,j,k)+d1(2,j,k)*e1(2,j,k)
        a1(3,j,k)=b1(3,j,k)*c1(3,j,k)+d1(3,j,k)*e1(3,j,k)
        a1(4,j,k)=b1(4,j,k)*c1(4,j,k)+d1(4,j,k)*e1(4,j,k)
        a1(5,j,k)=b1(5,j,k)*c1(5,j,k)+d1(5,j,k)*e1(5,j,k)
        a1(6,j,k)=b1(6,j,k)*c1(6,j,k)+d1(6,j,k)*e1(6,j,k)
        a1(7,j,k)=b1(7,j,k)*c1(7,j,k)+d1(7,j,k)*e1(7,j,k)
        a1(8,j,k)=b1(8,j,k)*c1(8,j,k)+d1(8,j,k)*e1(8,j,k)
      end do
   end do

end program catalog_no21

