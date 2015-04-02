
program TileLoop
    implicit none
    integer:: i, j, i1, j1,k
    integer, parameter :: N = 3500
    real:: a(N,N), b(N,N), c(N,N)
    real :: t1,t2,ta
    do j=1,N
        do i=1,N
            a(i,j)=i+j
            b(i,j)=i*j
            c(i,j)=0.0
        end do
    end do

    call cpu_time(t1)

    do k=1,100
        do j=1,N
            do i=1,N
                a(i,j)=a(i,j)+b(i,j)*a(i,j)
            end do
        end do
    end do

    call cpu_time(t2)
    ta=real(t2-t1)/100

    if (a(1,1) == 2.53530120E+30) then
        print *, "OK: ", a(1,1)
    else
        print *, "NG", a(1,1)
    endif

end program TileLoop


