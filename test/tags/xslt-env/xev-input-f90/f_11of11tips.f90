      module glob
      real, dimension(:), allocatable :: x
      contains
         subroutine sub( y )
         real, dimension(:) :: y
!$xev dir replace(acc,kernels,present_or_copy(y),present_or_copyin(x))
!$acc declare present(y,x)
!$xev dir remove
!$acc kernels
         do i = 1, ubound(y,1)
            y(i) = y(i) + x(i)
         enddo
!$acc end kernels
         end subroutine
      end module

      subroutine roo( z )
      use glob
      real :: z(:)
!$acc data copy(z) copyin(x)
      call sub( z )
!$acc end data region
      end subroutine

