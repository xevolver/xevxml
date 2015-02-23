module f05_check
  contains

    subroutine ca203(s,t,m,n)
    complex(kind=4),dimension(m,n) :: s,t
    print *,all(s.eq.t)
    end subroutine ca203

end module 
