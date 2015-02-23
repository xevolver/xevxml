        interface
           subroutine sub(char1,char2,vec)
     	   character(kind=1,len=7),dimension(:,:,:) :: char1,char2
     	   integer(kind=4),dimension(:) :: vec
           end subroutine 
        end interface
	character(kind=1,len=7),dimension(6,6,6) :: ch1,ch2
	integer(kind=4),dimension(6)  :: v
	integer(kind=4)               :: i,j,k
        do i=1,6
           do j=1,6
              do k=1,6
                 ch1(i,j,k) = 1_' '
                 ch2(i,j,k) = 1_' '
              end do
           end do
           v(i) = i
        end do
	end program 

	subroutine sub(ch1,ch2,v)
	character(kind=1,len=7),dimension(:,:,:) :: ch1,ch2
	integer(kind=4)::i,j,k
	integer(kind=4),dimension(:) :: v
        logical(kind=1)::flag
	end subroutine 
