! recursive and result disappears
integer recursive function s5(i) result(n)

  if (i <= 0) then
     n = 1
  else
     n = i * s5(i-1)
  end if

end function s5
