        subroutine sub_092_01
        type t2
        	integer::i(3)
        	real::r(3)
        	complex::comp(3)
        	character(kind=1,len=10)::ch(3)
        end type t2

        type(t2)::str2
        	
        namelist /name2/str2

        data str2/t2(1,1.0,(1.0,1.0),'abc')/

		write(*,*)'&NAME2 STR2=1,1,1,1.000000,1.000000,1.000000,&
		&(1.000000,1.000000),(1.000000,1.000000),(1.000000,1.000000),&
		&abc       abc       '
                write(*,*)'abc        /'
		write(*,nml=name2)
		write(*,*)

        end subroutine sub_092_01

