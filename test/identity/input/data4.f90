	subroutine sub3
	character(len=10)::i
	data ((i(j+k:10),j=1,10,5),k=5,6)/5*'abc'/
	end
