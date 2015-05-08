include ../../../lib/Makefile-xev.mk

TARGET_SRC = $(wildcard *.target.f90)
DIFF = $(TARGET_SRC:%target.f90=%diff.f90)

check: clean-translated $(DIFF)
	echo $(DIFF)

test:
	@echo "test"
	make  loop-3.diff.f90

