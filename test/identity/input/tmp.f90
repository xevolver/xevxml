SUBROUTINE ft09_005_01()
INTEGER :: id1
INTEGER, DIMENSION(10) :: it
INTEGER :: is1(10), is2(10), is3(10), is4(10)
INTEGER, DIMENSION(4) :: in
id1 = 1
it = (/(i,i = 1, 10)/)
OPEN (UNIT=10, FILE="out_ft09_005_01") 
WRITE (10, ERR=999) it
REWIND (UNIT=10) 
!	write(10,id=id1,err=999) it(2)...
!	write(10,id=id1,err=999) ...it(8)
!	write(10,id=id1,err=999) it(3)...it(8)
is1 = 0
is2 = 0
is3 = 0
is4 = 0
in = 0
READ (UNIT=10) is1
READ (UNIT=10) is2
READ (UNIT=10) is3
READ (UNIT=10) is4
CLOSE (UNIT=10) 
CALL check_ft09_005_01("ft09_005_01_01",in,(/10 * kind(it),9 * kind(it),8 * kind(16),6 * kind(it)/),4)
!	call check_ft09_005_01("ft09_005_01_01",in,(/40,36,32,24/),4)
CALL check_ft09_005_01("ft09_005_01_02",is1,it,10)
CALL check_ft09_005_01("ft09_005_01_03",is2,(/2,3,4,5,6,7,8,9,10,0/),10)
CALL check_ft09_005_01("ft09_005_01_04",is3,(/1,2,3,4,5,6,7,8,0,0/),10)
CALL check_ft09_005_01("ft09_005_01_05",is4,(/3,4,5,6,7,8,0,0,0,0/),10)
RETURN
999 PRINT *, "file I/O error"
END SUBROUTINE 

SUBROUTINE ft09_005_02()
INTEGER :: id1
INTEGER :: it1(5), it2(5)
INTEGER :: is1(10), is2(10), is3(10), is4(10)
INTEGER, DIMENSION(4) :: in
COMMON / cb / it1,it2
id1 = 1
it1 = (/1,2,3,4,5/)
it2 = (/6,7,8,9,10/)
OPEN (UNIT=10, FILE="out_ft09_005_02") 
REWIND (UNIT=10) 
!	write(10,id=id1,err=999) it1...it2
!	write(10,id=id1,err=999) it1(2)...it2
!	write(10,id=id1,err=999) it1...it2(3)
!	write(10,id=id1,err=999) it1(3)...it2(3)
is1 = 0
is2 = 0
is3 = 0
is4 = 0
in = 0
READ (UNIT=10) is1
!	wait(10,id=id1,num=in(1))
READ (UNIT=10) is2
!	wait(10,id=id1,num=in(2))
READ (UNIT=10) is3
!	wait(10,id=id1,num=in(3))
READ (UNIT=10) is4
!	wait(10,id=id1,num=in(4))
CLOSE (UNIT=10) 
CALL check_ft09_005_01("ft09_005_02_01",in,(/10 * kind(it1),9 * kind(it1),8 * kind(it1),6 * kind(it1)/),4)
CALL check_ft09_005_01("ft09_005_02_02",is1,(/1,2,3,4,5,6,7,8,9,10/),10)
CALL check_ft09_005_01("ft09_005_02_03",is2,(/2,3,4,5,6,7,8,9,10,0/),10)
CALL check_ft09_005_01("ft09_005_02_04",is3,(/1,2,3,4,5,6,7,8,0,0/),10)
CALL check_ft09_005_01("ft09_005_02_05",is4,(/3,4,5,6,7,8,0,0,0,0/),10)
RETURN
999 PRINT *, "file I/O error"
END SUBROUTINE 

