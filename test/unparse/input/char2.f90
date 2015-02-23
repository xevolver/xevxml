
subroutine call_701c
! 形状引継ぎ配列
! 部分配列
interface
    subroutine call_701_01c(u)
    integer      ,dimension(:)  :: u
    end subroutine call_701_01c
    subroutine call_701_02c(u)
    complex      ,dimension(:)  :: u
    end
    subroutine call_701_03c(u)
    character(kind=1,len=4),dimension(:) :: u
    end
end interface
integer      ,dimension(10)  :: is,it
complex      ,dimension(10)  :: cs,ct
character(kind=1,len=4),dimension(10) :: chs,cht
it = 1
ct = (2.3,4.5)
cht = 'ABCD'
is = 1
cs = (2.3,4.5)
chs = 'ABCD'
do i=1,10,2
  is(i) = 9
  cs(i) = (9.9,9.9)
  chs(i) = '9999'
enddo
call call_701_01c(it(1:10:2))
call call_701_02c(ct(1:10:2))
call call_701_03c(cht(1:10:2))
call check_701_01("701_01c",is,it,10)
call check_701_06("701_02c",cs,ct,10)
call check_701_09("701_03c",chs,cht,10)
end

subroutine call_701_01c(u)
integer      ,dimension(:)  :: u
u = 9
end
subroutine call_701_02c(u)
complex      ,dimension(:)  :: u
u = (9.9,9.9)
end
subroutine call_701_03c(u)
character(kind=1,len=4),dimension(:) :: u
u = '9999'
end

subroutine call_701b
! 暗黙のインタフェース
! ベクトル添字の部分配列
integer      ,dimension(10)  :: is,it
complex      ,dimension(10)  :: cs,ct
character(kind=1,len=4),dimension(10) :: chs,cht
integer      ,dimension(5)   :: v
data v/1,3,5,7,9/
it = 1
ct = (2.3,4.5)
cht = 'ABCD'
is = 1
cs = (2.3,4.5)
chs = 'ABCD'
do i=2,10,2
  it(i) = 9
  is(i) = 9
  ct(i) = (9.9,9.9)
  cs(i) = (9.9,9.9)
  cht(i) = '9999'
  chs(i) = '9999'
enddo
call call_701_01b(it(v),5)
call call_701_02b(ct(v),5)
call call_701_03b(cht(v),5)
call check_701_01("701_11b",it,is,10)
call check_701_06("701_12b",ct,cs,10)
call check_701_09("701_13b",cht,chs,10)
end

end
