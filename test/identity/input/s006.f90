program s6

outer: do i=1, 10
   inner: do j=1, 10
! name of do-loop disappears
      exit outer
   end do inner
end do outer

end program s6
