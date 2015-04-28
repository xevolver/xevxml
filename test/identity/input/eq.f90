module f05
  contains
    subroutine ck09(s,t)
    complex(kind=16) :: s,t
    if(s.eq.t) then
        print *,"EQ"
    else
        print *,"NE"
    endif
    end subroutine ck09

end module 
