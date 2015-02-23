	subroutine t03_034(n)
	logical(kind=1),dimension(n)         :: log11,log12
	logical(kind=1),dimension(15)        :: work1,work2,rslt
        where(log11 .eqv. log12)
           rslt = work1
        else where
           rslt = work2
        end where
	end subroutine t03_034
