    module mod
    type t1
        integer::i
        character(len=7)::ch
    end type
    end module

    program main
    use mod
    type(t1),dimension(10)::str
    str=t1(0,"")
    end program
    
