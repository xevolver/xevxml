
program catalog_no22

  integer,parameter :: n=139

  real*8 a1(n,n,n),b1(n,n,n),c1(n,n,n),d1(n,n,n),e1(n,n,n)

  !$xev loop_roll dim(3,k,1,n3)
  do j=1,n2
     !$xev loop_unroll step(8)
     do i=1,n1
        a1(i,j,1)=b1(i,j,1)*c1(i,j,1)+d1(i,j,1)*e1(i,j,1)
        a1(i,j,2)=b1(i,j,2)*c1(i,j,2)+d1(i,j,2)*e1(i,j,2)
        a1(i,j,3)=b1(i,j,3)*c1(i,j,3)+d1(i,j,3)*e1(i,j,3)
        a1(i,j,4)=b1(i,j,4)*c1(i,j,4)+d1(i,j,4)*e1(i,j,4)
        a1(i,j,5)=b1(i,j,5)*c1(i,j,5)+d1(i,j,5)*e1(i,j,5)
        a1(i,j,6)=b1(i,j,6)*c1(i,j,6)+d1(i,j,6)*e1(i,j,6)
        a1(i,j,7)=b1(i,j,7)*c1(i,j,7)+d1(i,j,7)*e1(i,j,7)
        a1(i,j,8)=b1(i,j,8)*c1(i,j,8)+d1(i,j,8)*e1(i,j,8)
      end do
   end do

end program catalog_no22

