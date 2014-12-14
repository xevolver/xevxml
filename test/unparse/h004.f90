subroutine sub
	interface
		function func() result(r)
			common/cmn/i
			real,dimension(i)::r
		end function
	end interface
	common/cmn/i
	real,dimension(i)::a
	a = (/func()/)
end
