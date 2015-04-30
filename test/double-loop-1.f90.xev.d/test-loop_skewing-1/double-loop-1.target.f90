program double_loop_1

    integer,parameter :: n=139

    real*8 A(n, n)

    !$xev loop_tag
    do i=1,n
        do j_new=i+1, i+n
            A(i,j_new-i)=A(i-1,j_new-i)+A(i,j_new-i-1)
        end do
    end do

end program double_loop_1
