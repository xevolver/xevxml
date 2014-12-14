    function fun() result(p)
    integer,pointer :: p
    call insub
    contains
    subroutine insub
    integer,save,target :: t
    t=999
    p => t
    end subroutine insub
    end function fun
