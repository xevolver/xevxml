
program triple_loop_1

    integer,parameter :: n=139

    real*8 A(n, n, n), B(n, n, n)

    !$xev loop_tag
    do k=1,n-1
        do j=1,n-1
            do i=1,n-1
                B(i,j,k) = A(i,j,k)
            end do
        end do
    end do

end program  triple_loop_1

