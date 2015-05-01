
program triple_loop_1

    integer,parameter :: n=139, w=10

    real*8 A(n), B(n)

    !$xev loop_tag
    do i=1,n-1
        A(i) = A(i) + B(i)
        if (w) then
            B(i)=0;
        endif
    end do

end program  triple_loop_1

