program s20

  type int2
     integer i, j
  end type int2

contains

  type(int2) function add(a, b)
    type(int2) :: a, b
    add%i = a%i + b%i
    add%j = a%j + b%j
  end function add

end program s20
