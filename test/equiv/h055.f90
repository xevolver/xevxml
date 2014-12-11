program f90_cnst_103
	type drv
		real,dimension(10)::r
	end type
	type(drv),target::w
	w%r = (/(real(i),i=1,10)/)
	!!t1 = drv1(itarget,w%r,ltarget(2:10:2))
	!t3(5)%ra = (/(real(i),i=1,10)/)
	end
