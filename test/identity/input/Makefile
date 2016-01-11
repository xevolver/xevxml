ROSECOMP=identityTranslator

ALL=*.f90 *.c *.F *.f *.f03

.SUFFIXES: .f90 .dif .xml

all: mod.rmod
	-@for i in $(ALL) ; \
	do if [ -f $$i ]; then \
	(cd module;XEV_TEST_MODE="true" sh ../../test.sh ../$$i) > /dev/null;\
	fi; done

mod.rmod : module/mod.f90
	@$(ROSECOMP) -c module/mod.f90 -rose:o module/rose_mod.f90

clean:
	-@rm -f module/*.rmod *.o rose_*.f90 *.mod module/*_postprocessed.f90
	make -C ../output clean
