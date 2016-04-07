#ifndef $(HYDIR)
#	override HYDIR = $(shell /bin/pwd)/./../
#endif
#sinclude	$(HYDIR)rules.make

export

SUBDIRS = src

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -w -C $@



.PHONY: clean
clean:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clean;done

