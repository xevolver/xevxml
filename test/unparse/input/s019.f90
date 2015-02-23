program s19

  integer :: a(10, 10)

  a = 0

  where (a < 0)
     a = 0
  end where

end program s19
