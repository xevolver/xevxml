        subroutine sub
        type t1
                sequence
                integer::i
                real::r
        end type t1

        type (t1),dimension(10)::str1,str2,str3
        data str2(1),str1(2),str3(2),str3(5),str3(10)/5*t1(1,1.0)/
        end 
