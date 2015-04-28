	function ft05(u) result(s)
	integer :: u,s
	select case(u)
	case(:3)
		s = 11
	case(4)
		s = 12
	case(5:7)
		s = 13
	case(8:)
		s = 14
	case default
		s = -1
	end select
	end
