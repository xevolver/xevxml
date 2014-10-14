module mod
character(len=2),dimension(20),parameter::num=(/"1 ","2 ","3 ","4 ","5 ",&
						&"6 ","7 ","8 ","9 ","10",&
						&"11","12","13","14","15",&
						&"16","17","18","19","20"/)
contains
subroutine sub(number)
	integer number
	character(len=80)::buf
		write(6,*) "		use mod_ft02_005_"//&
							&num(number)
		buf = trim(buf)//"::ent_ft02_005_"//num(number)
end subroutine
end module
