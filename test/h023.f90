module global
   interface aa
     module procedure a,b
   end interface
contains
   subroutine a()
   write(*,*)"a"
   endsubroutine  
   subroutine b(c)
   write(*,*)"b"
   end  subroutine
end module
