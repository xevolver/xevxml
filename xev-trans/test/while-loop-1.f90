
program while_loop_1

    integer,parameter :: n=139, i=0

    real*8 a(n)

    !$xev loop_tag
    hoge: do while(i<n)
        a(i) = 0
        i = i+1
    end do

end program while_loop_1

