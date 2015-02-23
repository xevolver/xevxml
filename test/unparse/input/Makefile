ROSECOMP=identityTranslator

ALL=*.f90 *.c *.F

.SUFFIXES: .f90 .dif .xml

all: mod.rmod
	-@for i in $(ALL) ; do sh ./testXevXml.sh $$i > /dev/null;  done

mod.rmod : module/mod.f90
	@$(ROSECOMP) -c module/mod.f90 -rose:o module/rose_mod.f90

clean:
	-@rm -f *.rmod *.o rose_*.f90 *.mod
	make -C ../output clean
