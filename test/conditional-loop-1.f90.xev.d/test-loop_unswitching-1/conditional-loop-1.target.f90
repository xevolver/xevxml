
program triple_loop_1

    integer,parameter :: n=139, w=10

    real*8 A(n), B(n)

    !$xev loop_tag
    if (w) then
        do i=1,n-1
            A(i) = A(i) + B(i)
            B(i)=0;
        end do
    else
        do i=1,n-1
            A(i) = A(i) + B(i)
        end do
    endif

end program  triple_loop_1

