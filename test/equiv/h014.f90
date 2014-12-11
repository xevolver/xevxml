module comp
	integer,parameter::vari_max = 2
	character(len=*),dimension(vari_max),parameter::type_str = (/& 
			&"character(kind=1)",&
			&"character(kind=2)"/)
	character(len=*),dimension(vari_max),parameter::variable_a = (/&
			&"ch  ",&
			&"nch "/)
end module

subroutine sub()
	use comp
	character(len=*),parameter::child="child"
	contains
	subroutine gen()
	character(len=80)::attribute
        attribute =",pointer"
        call gen_decl(type_str(vari_max)//&
				&"("//child//")",&
				&attribute,variable_a(vari_max))
endsubroutine
end
