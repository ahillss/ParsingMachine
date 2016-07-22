OUTDIR = bin
OUTFILE = $(OUTDIR)/tcltest

CC = gcc
EXT = c

CPPFLAGS :=
CFLAGS :=
LDFLAGS :=

#DEBUG_BUILD = -g
#DEBUG_TEST = gdb --eval-command=run

objs := $(patsubst %.$(EXT),$(OUTDIR)/%.o,$(wildcard *.$(EXT)))
deps := $(objs:.o=.dep)

.PHONY: all test
all: $(OUTFILE)

-include $(deps)

$(OUTDIR)/%.o: %.$(EXT)
	@mkdir -p $(@D)
	$(CC) $(DEBUG_BUILD) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@$(CC) $(CPPFLAGS) $(CFLAGS) -MM $< | sed -e '1,1 s|[^:]*:|$@:|' > $(OUTDIR)/$*.dep

$(OUTDIR)/parmac.o: ../parmac.c ../parmac.h
	$(CC) $(DEBUG_BUILD) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	
$(OUTFILE) : $(objs) $(OUTDIR)/parmac.o
	$(CC) $(DEBUG_BUILD) $^ $(LDFLAGS) -o $@

test: $(OUTFILE)
	@$(DEBUG_TEST) $(OUTFILE) tests/b.tcl

clean:
	@rm -f $(deps) $(objs) $(OUTFILE) $(OUTFILE).exe $(OUTDIR)/parmac.o
	@rmdir --ignore-fail-on-non-empty $(OUTDIR)
