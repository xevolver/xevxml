
program while_loop_1

    integer,parameter :: n=139, i=0

    real*8 a(n)

    !$xev loop_tag
    hoge: if (i<n) then
        do
            a(i) = 0
            i = i+1
            if  (i<n)  cycle
            exit
        end do
    endif

end program while_loop_1

