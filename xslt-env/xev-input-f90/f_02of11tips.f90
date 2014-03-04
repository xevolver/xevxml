
SUBROUTINE TEST02()
    !$xev var-type-chg ptn-001(SgArrayType,1,SgTypeInt)
    INTEGER found
    INTEGER i
    INTEGER, DIMENSION(300) :: A

    !$xev dir replace(acc, kernels, loop)
    !$acc kernels

    !$xev statement-del ptn-001
    i = 0
    !$xev whilestmt-chg ptn-001(do)
    do  while ( .not. found==300)
        i = i + 1
        if (A(i) .eq. 102) then
           found = i
        end if
    end do
    !$acc end region
    
    !$xev statement-add ptn-001(print,*,'Found_at',maxval(found) )
    return
END

