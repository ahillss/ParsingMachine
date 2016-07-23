OUTDIR = bin
OUTFILE = $(OUTDIR)/oldtcltest

CC = gcc
EXT = c

CPPFLAGS :=
CFLAGS :=
LDFLAGS :=

objs := $(patsubst %.$(EXT),$(OUTDIR)/%.o,$(wildcard *.$(EXT)))
deps := $(objs:.o=.dep)

.PHONY: all test
all: $(OUTFILE)

-include $(deps)

$(OUTDIR)/%.o: %.$(EXT)
	@mkdir -p $(@D)
	$(CC) $(DEBUG_BUILD) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@$(CC) $(CPPFLAGS) $(CFLAGS) -MM $< | sed -e '1,1 s|[^:]*:|$@:|' > $(OUTDIR)/$*.dep
	
$(OUTFILE) : $(objs)
	$(CC) $(DEBUG_BUILD) $^ $(LDFLAGS) -o $@

test: $(OUTFILE)
	@$(DEBUG_TEST) $(OUTFILE) tests/b.tcl

clean:
	@rm -f $(deps) $(objs) $(OUTFILE) $(OUTFILE).exe
	@rmdir --ignore-fail-on-non-empty $(OUTDIR)
