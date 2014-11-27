
subroutine simple_conditional1

    if (SUM.eq.5) then
    SUM = SUM + 5
    endif

    if ((I+3).eq.J) then
    SUM = SUM + 5
    endif

end
