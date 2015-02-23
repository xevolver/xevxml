module mod_ft07_003_3
interface operator(.lsub.)
module procedure uminus_b0
end interface
contains
function uminus_b0(b1) result(b2)
logical(kind=1),intent(in) :: b1
logical(kind=1) :: b2
b2 = .not.b1
end function uminus_b0
end module mod_ft07_003_3
subroutine sub_ft07_003_3
use mod_ft07_003_3
logical(kind=1) :: b1,b2 = .true._1
b1 = .lsub.b2
end subroutine sub_ft07_003_3
