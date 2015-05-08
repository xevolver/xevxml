
program triple_loop_and_if

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

    !$xev loop_tag2
    if ( A(1,1,1) .gt. B(1,1,1) ) then
       B(i,j,k) = A(i,j,k)
    end if

end program  triple_loop_and_if

