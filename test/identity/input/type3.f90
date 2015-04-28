        module type
	type t2
		integer::i(10)
		integer,pointer::ip(:)
	end type t2

        type t1
                integer::i(10)
		type (t2)::str
        end type t1
        end module type


         subroutine sub(str)
            use type
            type(t1),pointer,dimension(:)::str
			do l=1,10
				do k=1,10
					str(l)%i(k) =  1
					str(l)%str%i(k) =  1
					str(l)%str%ip(k) = 1
				end do
			end do
         end subroutine sub

