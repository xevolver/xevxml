SUBDIRS=src 
all:
	list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir;\
	done
clean:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir clean;\
	done
check:
	make -C test check


