
subroutine simple_conditional1

    !$xev conditional001 if(SUM.eq.5)
    SUM = SUM + 5
    !$xev end conditional

    !$xev conditional002 if((I+3).eq.J)
    SUM = SUM + 5
    !$xev end conditional

end
