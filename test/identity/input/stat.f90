	subroutine ft09_001_01
        integer(kind=2)         ,allocatable,dimension(:) :: ht
	integer,dimension(15)   :: ic,id
	ic = 999
	id = 999
	allocate(ht(5), stat=ic(1))
	deallocate(ht, stat=id(1))
	end
