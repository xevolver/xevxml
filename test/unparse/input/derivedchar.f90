	program ft03_021
        type struc
	   character(kind=1,len=7),dimension(15) :: ch1,ch2
        end type struc
        type(struc)   :: ty1,ty2
        ty2%ch1(:)(5:7) = ty1%ch1(:)(1:3)
        ty2%ch2(:)(5:7) = ty1%ch2(:)(1:3)
	end program 
