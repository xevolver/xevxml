	integer,dimension(5)::a1,a2
        integer,dimension(25)::ia1,ia2(20)
        ii=1; jj=5; kk=2;
        a1 = (/(10,i=1,10,2)/)
	a2 = (/(2,i=ii,jj,kk)/)
        ia1 = (/((i,i=1,10,2),j=1,10,2)/)
        ia2 = (/(ia1(1:5),i=1,4)/)
end
