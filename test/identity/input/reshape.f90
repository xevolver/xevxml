subroutine sub
real::a2(10,20)
a2 = reshape((/((real(i),i=1,10),j=1,20)/),(/10,20/))
end
