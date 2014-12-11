program upacs_convention
  implicit none
  type block 
     integer in,jn,kn
     double precision,pointer::dq(:,:,:,:)
     double precision,pointer::inv_vol(:,:,:)
  end type block
  type face
     double precision,pointer:: flux(:)
  end type face
  type(block) blk
  type(face) cface(1024,1024,1024)
  integer n,i,j,k,im,jm,km
  integer:: bdtv_nFlowVar=1024
  integer,dimension(3) :: idelta
  double precision,pointer ::dq(:,:,:,:),inv_vol(:,:,:)
  double precision,pointer :: flux(:,:,:,:)
  integer :: in,jn,kn

  blk%in = 1024
  blk%jn = 1024
  blk%kn = 1024
  allocate(blk%dq(1024,1024,1024,1024))
  allocate(blk%inv_vol(1024,1024,1024))
  do k=1,1024
     do j=1,1024
        do i=1,1024
           allocate(cface(i,j,k)%flux(1024))
           do n=1,1024
              cface(i,j,k)%flux(n) = n
              blk%dq(i,j,k,n) = n
              blk%inv_vol(i,j,k) = i*j*k
           end do
        end do
     end do
  end do


  dq=>blk%dq
  inv_vol=>blk%inv_vol
  in = blk%in
  jn = blk%jn
  kn = blk%kn
  do k = 1, kn
     do j = 1, jn
        do i = 1, in
           flux( :, i, j, k) = cface(i, j, k)%flux(:)
        end do
     end do
  end do
  do n=1,bdtv_nFlowVar
     do k=1,kn
        do j=1,jn
           do i=1,in
              im=i-idelta(1)
              jm=j-idelta(2)
              km=k-idelta(3)
              dq(i,j,k,n) = dq(i,j,k,n) - inv_vol(i,j,k) &
                   * ( flux(n,i,j,k) - flux(n,im,jm,km) )
           end do
        end do
     end do
  end do
end program upacs_convention
