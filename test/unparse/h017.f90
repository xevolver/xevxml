module comp
	integer,parameter::vari_max = 15
	character(len=*),dimension(vari_max),parameter::type_str = (/& 
			&"character(kind=1)",&
			&"character(kind=2)",&
			&"logical(kind=1)  ",&
			&"logical(kind=4)  ",&
			&"logical(kind=8)  ",&
			&"integer(kind=2)  ",&
			&"integer(kind=4)  ",&
			&"integer(kind=8)  ",&
			&"real(kind=4 )    ",&
			&"real(kind=8 )    ",&
			&"real(kind=16)    ",&
			&"complex(kind=4 ) ",&
			&"complex(kind=8 ) ",&
			&"complex(kind=16) ",&
			&"type             "&
			&/)
end module
