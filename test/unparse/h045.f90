subroutine sub
	type drv1
		integer::i
		real::r
	end type
end
subroutine sub2
	type drv1
		integer::i(10)
		real::r(10)
	end type
	type drv2
		integer::i(10)
		type(drv1)::t(10)
	end type
	type(drv2),dimension(10)::t2
	logical::ng
	do i=1,10
		do j=1,10
			do k=1,10
				if (t2(i)%t(j)%i(k).ne.i*j*k) ng = .true.
			end do
		end do
	end do
end
