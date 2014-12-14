program f90_cnst_103
	type drv1
		integer,dimension(:),pointer::iptr
		real,dimension(:),pointer::rptr
		logical(kind=8),dimension(:),pointer::lptr
	end type
	type drv2
		real,dimension(10)::r
	end type
	type drv3
		integer::i
		real,dimension(10)::ra
		complex,dimension(20)::ca
	end type
	type drv4
		integer,dimension(:),pointer::ip
		real,dimension(:),pointer::rp
		complex,dimension(:),pointer::cp
	end type
	integer,dimension(10),target::it
	logical(kind=8),dimension(30),target::lt
	type(drv1)::t1
	type(drv2),target::w
	type(drv3),dimension(10),target::t3
	type(drv4)::t4
!	t1 = drv1(it,w%r,lt(2:10:2))
!	t4 = drv4(t3%i,t3(5)%ra,t3(6)%ca(1:20:2))
	end
