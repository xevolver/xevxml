        interface
           subroutine sub(char1,char2,vec)
     	   character(kind=1,len=7),dimension(:) :: char1,char2
     	   integer(kind=4),dimension(:) :: vec
           end subroutine sub
        end interface
	character(kind=1,len=7),dimension(15) :: ch1,ch2
	integer(kind=4),dimension(15) :: v
        do n=1,15
           ch1(n) = 1_' '
           ch2(n) = 1_' '
           v(n) = 0
        end do
	end program 
!
	subroutine sub(ch1,ch2,v)
	character(kind=1,len=7),dimension(:) :: ch1,ch2
	integer(kind=4)::i,j,k
	integer(kind=4),dimension(:) :: v
        logical(kind=1)::flag
	end subroutine sub
