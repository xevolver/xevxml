         function func(logic)
         logical::logic,func,ent
			func=(.not. logic)
			return
         entry ent(logic)
			ent=(.not. logic)
			return
         end function

