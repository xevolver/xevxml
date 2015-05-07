DIR=triple-loop-1.f90.xev.d
JSON=$(DIR)/*.json

all:
	-@for i in $(JSON) ; do sh ./test.sh triple-loop-1.f90 $$i > /dev/null;  done
clean:
	-@rm -f $(DIR)/AFTER_* $(DIR)/*.log $(DIR)/*.dif $(DIR)/*.xml $(DIR)/*.xsl
