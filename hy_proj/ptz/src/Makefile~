#ifndef $(HYDIR)
#	override HYDIR = $(shell /bin/pwd)/./../../
#endif
#sinclude	$(HYDIR)rules.make

CFLAGS +=  $(HYPTZINC)
LDFLAGS = $(HYPTZLINKNAME) $(HYPTZLINKPATH)


LIBNAME = $(HYPTZLIBNAME)
AR_FLAGS =


SRCS := $(wildcard *.c)
SRC_FILES := $(notdir $(SRCS))
OBJ_FILES := $(patsubst %c,%o,$(SRC_FILES))

.PHONY:all
all: dep otoar
dep:
%.d: %.c
	@rm -f $@; \
	$(CC) $< -MM $(HYPTZINC)> $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


otoar: $(OBJ_FILES)
	$(AR) cr $(LIBNAME) $(OBJ_FILES)
	@cp $(LIBNAME) $(HYLIBPATH)

#$(OBJ_FILES): $(SRC_FILES) 
#	@echo -e "building: $(notdir $^) \n\t please wait ...\n"
#	$(CC) $(CFLAGS) -c $^


sinclude $(SRCS:.c=.d)
.PHONY:clean
clean:
	-rm -f *.o *.a *.d



