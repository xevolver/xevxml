subroutine checkh(n,t,r)
character(len=*) :: n
integer(kind=2) :: t,r
if(t.eq.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkh
subroutine checki(n,t,r)
character(len=*) :: n
integer(kind=4) :: t,r
if(t.eq.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checki
subroutine checkl(n,t,r)
character(len=*) :: n
integer(kind=8) :: t,r
if(t.eq.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkl
subroutine checkr(n,t,r)
character(len=*) :: n
real(kind=4) :: t,r
if(abs(t-r).lt.(1.0e-4))then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkr
subroutine checkd(n,t,r)
character(len=*) :: n
real(kind=8) :: t,r
if(abs(t-r).lt.(1.0d-8))then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkd
subroutine checkq(n,t,r)
character(len=*) :: n
real(kind=16) :: t,r
if(abs(t-r).lt.(1.0q-12))then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkq
subroutine checkc(n,t,r)
character(len=*) :: n
complex(kind=4) :: t,r
if ((abs(real(t)-real(r)).lt.(1.0e-4)).or.(abs(aimag(t)-aimag(r)).lt.(1.0e-4)))then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkc
subroutine checkcd(n,t,r)
character(len=*) :: n
complex(kind=8) :: t,r
if ((abs(real(t)-real(r)).lt.(1.0e-8)).or.(abs(aimag(t)-aimag(r)).lt.(1.0e-8)))then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkcd
subroutine checkcq(n,t,r)
character(len=*) :: n
complex(kind=16) :: t,r
if ((abs(real(t)-real(r)).lt.(1.0e-12)).or.(abs(aimag(t)-aimag(r)).lt.(1.0e-12)))then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkcq
subroutine checkb(n,t,r)
character(len=*) :: n
logical(kind=1) :: t,r
if (t.eqv.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkb
subroutine checks(n,t,r)
character(len=*) :: n
logical(kind=4) :: t,r
if (t.eqv.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checks
subroutine checkw(n,t,r)
character(len=*) :: n
logical(kind=8) :: t,r
if (t.eqv.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkw
subroutine checkch(n,t,r)
character(len=*) :: n
character(kind=1,len=*) :: t,r
if (t.eq.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checkch
subroutine checknch(n,t,r)
character(len=*) :: n
character(kind=2,len=*) :: t,r
if (t.eq.r)then
   write(*,*)n," ok"
else
   write(*,*)n," ng"
   write(*,*)" true value: ",t
   write(*,*)" real value: ",r
endif
end subroutine checknch
