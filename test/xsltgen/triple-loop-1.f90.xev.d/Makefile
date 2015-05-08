JSON=*.json

all:
	-@for i in $(JSON) ; do sh ../test.sh ../triple-loop-1.f90 $$i;  done
clean:
	-@rm -f AFTER_* *.log *.dif *.xml *.xsl *~ 
