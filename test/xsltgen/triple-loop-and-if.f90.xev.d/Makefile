JSON=*.json

all:
	-@for i in $(JSON) ; do sh ../test.sh ../triple-loop-and-if.f90 $$i;  done
clean:
	-@rm -f AFTER_* *.log *.dif *.xml *.xsl *~ 
