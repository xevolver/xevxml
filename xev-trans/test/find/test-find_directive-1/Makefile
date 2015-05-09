include ../../../lib/Makefile-xev.mk


check: check-ok check-ng

check-ok:
	@cat ifstmt_OK.xml | $(XSLTEXEC) test_find_directive.xsl | grep FOUND_FOUND_FOUND
check-ng:
	@!(cat ifstmt_NG.xml | $(XSLTEXEC) test_find_directive.xsl | grep FOUND_FOUND_FOUND)

test:
	@echo "test"
	make double-loop-1.f90

