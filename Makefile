SUBDIRS=src
all:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir;\
	done
clean:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir clean;\
	done
	make -C test clean
install:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir install;\
	done
check:
	make -C test all
doc:
	-@doxygen docs/doxygen/Doxygen.conf
