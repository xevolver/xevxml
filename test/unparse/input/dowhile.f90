	subroutine sub5_3
	integer,dimension(12) :: is2
	do while(m.lt.4)
		m = m + 1
	    n = 0
		do while(n.lt.3)
			n = n + 1
			k = (m - 1) * 3 + n
			if ((k.ge.4).and.(k.le.8)) cycle
			is2(k) = 1
		enddo
	enddo
	end

