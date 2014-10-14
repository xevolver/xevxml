        subroutine sub
		integer::i1
		namelist/name1/i1
		data i1/1/

		write(*,nml=name1)

        end subroutine 
