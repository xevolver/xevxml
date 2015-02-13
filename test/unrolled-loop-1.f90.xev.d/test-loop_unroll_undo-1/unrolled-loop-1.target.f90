
program UnrollTest
    implicit none
    integer :: loopUpperBound
    integer:: i, j, k, m
    integer, parameter :: N = 512
    real:: a(N,N), b(N,N), c(N,N)
    real :: t1,t2,ta
    do j=1,N
        do i=1,N
            a(i,j)=i+j
            b(i,j)=i*j
        end do
    end do

    call cpu_time(t1)
    do m=1,100
        do i=1,N
            do j=1,N
                c(i,j)=0.0
                loopUpperBound = N
                do k=1, loopUpperBound
                    c(i,j)=c(i,j)+b(k,j)*a(i,k)
                end do
            end do
        end do
    end do
    call cpu_time(t2)
    ta=real(t2-t1)/100
    print *,  ta
end program UnrollTest
