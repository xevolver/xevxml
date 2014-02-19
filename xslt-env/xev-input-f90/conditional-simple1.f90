
subroutine simple_conditional1

    !$xev conditional if(SUM.eq.5)
    SUM = SUM + 5
    !$xev end conditional

    !$xev conditional if((I+3).eq.J)
    SUM = SUM + 5
    !$xev end conditional

end
