
program triple_loop_1

    integer,parameter :: n=139

    real*8 A(n, n, n), B(n, n, n)

    do j=1,n-1
        do i=1,(n-1)*(n-1)
            B(i,j,1) = A(i,j,1)
        end do
    end do

end program  triple_loop_1

