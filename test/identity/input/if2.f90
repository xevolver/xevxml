        subroutine sub
		logical :: flag(8) = .true.
		integer :: i1(0)
		integer :: i2(0)
		integer :: i3(4)
		integer :: i4(4)

		integer :: j1(0)
		integer :: j2(0)
		integer :: j3(4)
		integer :: j4(4)

		integer :: k1(0)
		integer :: k2(0)
		integer :: k3(4)
		integer :: k4(4)

		integer :: l1(0)
		integer :: l2(0)
		integer :: l3(4)
		integer :: l4(4)


		data i3(3),i4(3),i1,i2/1,2/
		data j3(3),j4(3),j1,j2/0*9,0*8,3,4/
		data k3(3),k4(3),k1,k2/5,6,0*2,0*4,0*9/
		data l3(3),l4(3),l1,l2/0*11,0*12,7,0*9,0*10,8,0*4,0*5/

		if (i3(3) .ne. 1) then
			flag(1) = .false.
		end if
		if (i4(3) .ne. 2) then
			flag(2) = .false.
		end if

		if (j3(3) .ne. 3) then
			flag(3) = .false.
		end if
		if (j4(3) .ne. 4) then
			flag(4) = .false.
		end if

		if (k3(3) .ne. 5) then
			flag(5) = .false.
		end if
		if (k4(3) .ne. 6) then
			flag(6) = .false.
		end if

		if (l3(3) .ne. 7) then
			flag(7) = .false.
		end if
		if (l4(3) .ne. 8) then
			flag(8) = .false.
		end if


        do j=1,8
                if(flag(j)) then
                        write(*,100)j
                else
                        write(*,200)j
                end if
        end do
100     format("f90_data_061 #",i2," OK")
200     format("f90_data_061 #",i2," NG")
        end
