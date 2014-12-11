SUBDIRS=src
all:
	list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir;\
	done
clean:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir clean;\
	done
install:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir install;\
	done
check:
	make -C test check
doc:
	doxygen docs/doxygen/Doxygen.conf
