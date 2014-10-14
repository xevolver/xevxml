	real,dimension(4)::r
	r = (/0.1,a,sin(0.3),func2(0.4)/)
end
function func2(r)
	func2 = r
end
