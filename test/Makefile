ROSECOMP=identityTranslator

ALL=s???\.f90 s???\.c h???\.f90

.SUFFIXES: .f90 .dif .xml

all: mod.rmod 
	-@for i in $(ALL) ; do sh ./testXevXml.sh $$i > /dev/null;  done

mod.rmod : mod.f90
	@$(ROSECOMP) -c mod.f90

clean:
	-@rm -f $(XML) $(DIF) a.out rose_*.f90 xev_*.f90 rose_*.c xev_*.c *.o *~ *.rmod *.mod s10.xml s10.dif *.log *.dif *.xml

