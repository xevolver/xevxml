
program triple_loop_1

    integer,parameter :: n=139

    real*8 A(n), B(n), C(n), D(n)

    !pragma xev loop_tag
    do i=1,n
        B(i) = A(i)
        D(i) = C(i)
    end do

end program  triple_loop_1

