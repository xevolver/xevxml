
program double_loop_1

    integer,parameter :: n=139

    real*8 A(n, n)

    !$xev loop_tag
    do i=1,n
        do j=1,n
            A(i,j)=A(i-1,j)+A(i,j-1)
        end do
    end do

end program double_loop_1
