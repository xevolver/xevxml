      module glob
      real, dimension(:), allocatable :: x
      contains
         subroutine sub( y )
         real, dimension(:) :: y
!$xev dir add(acc, present_or_copy(y) present_or_copyin(x)
!$xev dir del(delete)
!$acc declare present(y,x)
!$xev dir del(delete)
!$acc kernels
         do i = 1, ubound(y,1)
            y(i) = y(i) + x(i)
         enddo
!$acc end kernels
         end subroutine
      end module

!$xev parameter add(zz)
      subroutine roo( z )
      use glob
!$xev parameter add(zz)
      real :: z(:)
!$acc data copy(z) copyin(x)
!$xev function-call copy-001(zz)
      call sub( z )
!$acc end data region
      end subroutine
