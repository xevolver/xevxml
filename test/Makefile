SUBDIRS=custom unparse xslt

all:
	list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir;\
	done
clean:
	-@list='$(SUBDIRS)';for subdir in $$list; do \
	make -C $$subdir clean;\
	done
