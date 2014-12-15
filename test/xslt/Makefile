SRC2XML=../../src/src2xml
XML2SRC=../../src/xml2src
XSLTEXE=../../src/xsltexec

LOOP2=chill_split loop_fission
LOOP3=chill_fuse loop_fusion
TRIPLE=\
loop_collapse \
loop_interchange \
loop_tile-1 \
loop_unroll \
chill_permute-1 \
chill_tile-1 \
chill_unroll-chill_unroll_jam \
chill_unroll \
chill_unroll_jam-chill_unroll \

all:
	@mkdir -p output
	@($(SRC2XML) input/loop-2.f90 | $(XML2SRC) output/loop-2.f90)
	@list='$(LOOP2)'; for r in $$list; do \
	echo "test-$$r.xsl : input/loop2.f90 to output/test-$$r.f90 "; \
	($(SRC2XML) input/loop-2.f90 | $(XSLTEXE) rule/user/test-$$r.xsl | $(XML2SRC) output/test-$$r.f90) \
	done
	@($(SRC2XML) input/loop-3.f90 | $(XML2SRC) output/loop-3.f90)
	@list='$(LOOP3)'; for r in $$list; do \
	echo "test-$$r.xsl : input/loop2.f90 to output/test-$$r.f90 "; \
	($(SRC2XML) input/loop-3.f90 | $(XSLTEXE) rule/user/test-$$r.xsl | $(XML2SRC) output/test-$$r.f90) \
	done
	@($(SRC2XML) input/triple-loop-1.f90 | $(XML2SRC) output/triple-loop-1.f90)
	@list='$(TRIPLE)'; for r in $$list; do \
	echo "test-$$r.xsl : input/loop2.f90 to output/test-$$r.f90 "; \
	($(SRC2XML) input/triple-loop-1.f90 | $(XSLTEXE) rule/user/test-$$r.xsl | $(XML2SRC) output/test-$$r.f90) \
	done
clean:
	-@rm -f output/*
