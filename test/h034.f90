	module type
	type t1
	  integer::i
	end type t1
	end module

	function func(str) result (res1)
	use type
	type(t1)::str,res1,res2
	res1%i=str%i+1
	return
	entry ent(str) result(res2)
	res2%i=str%i+1
	return
	end function
