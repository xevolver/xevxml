include ../lib/Makefile-xev.mk


default:
	@echo "useage: make {check-env, input-src, check-all}"

check-env:
	@hostname
	@$(XEVXML_ENV) which src2xml


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
	@-mkdir -p $*.f90.xev.d
# src2xml
	@-mkdir -p $*.f90.xev.d/src2xml
	$(XEVXML_ENV) src2xml $*.f90 > $*.f90.xev.d/src2xml/$*.xml
	$(XEVXML_ENV) xml2src < $*.f90.xev.d/src2xml/$*.xml $*.f90.xev.d/src2xml/$*.f90
# dir2xml
#	@-mkdir -p $*.f90.xev.d/dir2xml
#	$(XEVXML_ENV) dir2xml ../dir-defs.xml < $*.f90.xev.d/src2xml/$*.xml > $*.f90.xev.d/dir2xml/$*.xml
#	$(XEVXML_ENV) xml2src < $*.f90.xev.d/dir2xml/$*.xml	 $*.f90.xev.d/dir2xml/$*.f90
	