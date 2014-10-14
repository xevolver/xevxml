    function fun() result(str)
    type t1
    	integer::i
    	character(len=5)::ch
    end type
    type(t1)::str
    call insub
    contains
    subroutine insub
    str=t1(999,"abc")
    end subroutine insub
    end function fun
