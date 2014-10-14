subroutine sub
	type drv
		real::r
		integer,dimension(3)::i
		character(len=5),dimension(3)::ch
	end type
	type(drv)::t2,t3,t4,t1
	type(drv),dimension(3)::t5,t6
	data t2%i / 3*1 /
	data t3%i / 1,2,3 /
	data t4%i(2) / 4 /
	data t1%i(1:3:2)/ 5,6 /
	data t5(2)%ch(1:3:2)(4:5) / 2*"ab" /
	data t6(1:3:2) / 2*drv(4,5.0,"ab") /
end
