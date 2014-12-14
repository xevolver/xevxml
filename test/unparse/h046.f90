subroutine sub_ft02_019_3
	type drv1
		integer::i
		real::r
	end type
	type(drv1)::t1
	open(unit=11,file="open_test_1",iostat=t1%i)
end
