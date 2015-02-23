        type struc
	   character(kind=1,len=7) :: ch1,ch2
        end type struc
        type subscr
	   integer(kind=4),dimension(15) :: s
        end type subscr
        type(struc),dimension(15) :: ty1,ty2
        type(subscr)              :: v
	integer(kind=4)::i,j
        logical(kind=1)::flag
        do i=1,15
           ty2(i)%ch1 = 1_' '
           ty2(i)%ch2 = 1_' '
           v%s(i)=i
        end do
        ty2(v%s) = ty1(v%s)
	end program 
