
program triple_loop_1

    integer,parameter :: n=139

    real*8 A(n), B(n)

    !$xev loop_tag
    do i=n,1,-1
        B(i) = A(i)
    end do

end program  triple_loop_1

