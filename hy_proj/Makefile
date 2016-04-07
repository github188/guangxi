sinclude	./rules.make

export


SUBDIRS = $(HYPTZPATH)
SUBDIRS += $(HYIOPATH)
SUBDIRS += $(HYCONFIGPATH) 
SUBDIRS += $(HYBASEPATH)

ifeq ($(PLATFORM), dm6467)
#    SUBDIRS +=$(HYAPPPATH) $(HY_DEC_APPPATH)
endif


.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -w -C $@


.PHONY: clean
clean:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clean;done




