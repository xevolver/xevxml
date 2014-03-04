SUBROUTINE TEST02()
INTEGER, DIMENSION(300) :: found
INTEGER :: i
INTEGER, DIMENSION(300) :: A
!pragma acc kernels loop 
doi=1,Nif(A(i).eq.102)thenfound(i)=ielsefound(i)=0endifenddoprint*,'Found_at',maxval(found)
    !$acc end region
RETURN
END SUBROUTINE 

