include ../lib/Makefile-xev.in


default:
	@echo "useage: make {check-env, input-src, check-all}"

check-env:
	@hostname
	@$(XEVXML_ENV) ; which src2xml


TEST_SRCS = $(wildcard *.f90)
XEV_DIRS = $(TEST_SRCS:%.f90=%.f90.xev.d)
TEST_DIRS = $(wildcard *.f90.xev.d/test-*)

input-src: $(TEST_SRCS:%.f90=%.f90.input-src)

check-all: input-src
	@$(foreach dir,$(TEST_DIRS),echo $(dir); cd $(dir); make check; cd ../..)
	@wc *.xev.d/test-*/*.diff.* *.xev.d/test-*/*.diff2.*

clean-translated-all:
	@$(foreach dir, $(TEST_DIRS), echo $(dir); cd $(dir); make clean-translated; cd ../..)

#clean-generated-all: clean-translated-all
clean-generated-all:
	$(foreach dir, $(XEV_DIRS), echo $(dir); cd $(dir); rm -rf src2xml dir2xml; cd ../)

#input-src: loop-1.f90.input-src loop-2.f90.input-src loop-3.f90.input-src triple-loop-1.f90.input-src
#input-src: $(INPUTS)

%.f90.input-src:
	@-mkdir $*.f90.xev.d
# src2xml
	@-mkdir $*.f90.xev.d/src2xml
	$(XEVXML_ENV) ; src2xml $*.f90 > $*.f90.xev.d/src2xml/$*.xml
	$(XEVXML_ENV) ; xml2src < $*.f90.xev.d/src2xml/$*.xml $*.f90.xev.d/src2xml/$*.f90
# dir2xml
	@-mkdir $*.f90.xev.d/dir2xml
	$(XEVXML_ENV) ; dir2xml ../dir-defs.xml < $*.f90.xev.d/src2xml/$*.xml > $*.f90.xev.d/dir2xml/$*.xml
	$(XEVXML_ENV) ; xml2src < $*.f90.xev.d/dir2xml/$*.xml	 $*.f90.xev.d/dir2xml/$*.f90
	
####################
# verify

verify:
	gfortran -o  tiled-loop-1.verify tiled-loop-1.f90 && ./tiled-loop-1.verify 

####################
# tags

test:
	echo $(INPUTS) 
#	triple-loop-1.f90


test-5:
	@echo "test-5"
# test-5
	@-mkdir triple-loop-1.f90.xev.d/test-5
	$(XEVXML_ENV) ; xsltrans triple-loop-1.f90.xev.d/user/test-5.xsl < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.xml > triple-loop-1.f90.xev.d/test-5/triple-loop-1.xml
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-5/triple-loop-1.xml	 triple-loop-1.f90.xev.d/test-5/triple-loop-1.f90

test-5-out-f:
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-5/triple-loop-1.out.xml triple-loop-1.f90.xev.d/test-5/triple-loop-1.out.f90

test-CHiLL-comp-1: input-src
# test-1
	@-mkdir triple-loop-1.f90.xev.d/test-1
	$(XEVXML_ENV) ; xsltrans triple-loop-1.f90.xev.d/user/test-1.xsl < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.xml > triple-loop-1.f90.xev.d/test-1/triple-loop-1.xml
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-1/triple-loop-1.xml	 triple-loop-1.f90.xev.d/test-1/triple-loop-1.f90
# test-2
	@-mkdir triple-loop-1.f90.xev.d/test-2
	$(XEVXML_ENV) ; xsltrans triple-loop-1.f90.xev.d/user/test-2.xsl < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.xml > triple-loop-1.f90.xev.d/test-2/triple-loop-1.xml
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-2/triple-loop-1.xml	 triple-loop-1.f90.xev.d/test-2/triple-loop-1.f90
# test-3
	@-mkdir triple-loop-1.f90.xev.d/test-3
### xsltrans	
	$(XEVXML_ENV) ; xsltrans triple-loop-1.f90.xev.d/user/test-3.xsl < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.xml > triple-loop-1.f90.xev.d/test-3/triple-loop-1.xml
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-3/triple-loop-1.xml	 triple-loop-1.f90.xev.d/test-3/triple-loop-1.f90
### xslproc
#	$(XEVXML_ENV) ; xslproc triple-loop-1.f90.xev.d/user/test-3.xsl < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.xml > triple-loop-1.f90.xev.d/test-3/triple-loop-1-xslproc.xml
#	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-3/triple-loop-1-xslproc.xml	 triple-loop-1.f90.xev.d/test-3/triple-loop-1-xslproc.f90
# test-4
	@-mkdir triple-loop-1.f90.xev.d/test-4
	$(XEVXML_ENV) ; xsltrans triple-loop-1.f90.xev.d/user/test-4.xsl < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.xml > triple-loop-1.f90.xev.d/test-4/triple-loop-1.xml
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/test-4/triple-loop-1.xml	 triple-loop-1.f90.xev.d/test-4/triple-loop-1.f90

xml2src-out:
	$(XEVXML_ENV) ; xml2src < triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.out.xml triple-loop-1.f90.xev.d/dir2xml/triple-loop-1.out.f90

clean:
	-rm triple-loop-1.f90.xev.d/test-1/* triple-loop-1.f90.xev.d/test-2/* triple-loop-1.f90.xev.d/test-3/* triple-loop-1.f90.xev.d/test-4/*

xev:
	@echo "xev"
	mkdir triple-loop-1.f90.xev.d
	