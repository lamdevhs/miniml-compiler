SOURCES = stack.c machine.c value.c enums.c ccam.h
PAUSE = read -n 1 -s -p "make: Compiled. Press any key to run." && echo
# default values:
in = gen.c
out = gen.out

# default operation to perform:
all: build

# usage: if you have generated a "foo.c" file with the compiler written in caml:
#	  (normal mode)  $ make build in=foo.c out=foo.out
#   (debug mode)   $ make build in=foo.c out=foo.out DBG=y
#   (memory trace) $ make build in=foo.c out=foo.out MEM=y
# and you can combine DBG=y and MEM=y together too.
# default values if not specified: in=gen.c out=gen.out
build: $(in) runtime.c $(SOURCES)
	gcc -Wall $(if $(MEM),-DTRACE_MEMORY,) $(if $(DBG),-DDEBUGMODE,) \
	 -o $(out) $^

# builds then runs a miniml program on top of the CCAM (CAM written in C).
# usage: same as for 'make build'.
run: build
	$(PAUSE) && ./$(out)

# launch the unit tests for the CCAM. usage:
#   (silent)  $ make tests
#   (verbose) $ make tests V=y
tests: unit-tests.c testing.c testing.h $(SOURCES)
	gcc -Wall $(if $(V),-DVERBOSE,) -o unit-tests.out $^ \
	 && $(PAUSE) \
	 && ./unit-tests.out

# dev experiments and tests:
dev: dev.c $(SOURCES)
	gcc -Wall -o dev.out $^ \
	 && $(PAUSE) \
	 && ./dev.out

.PHONY: clean

clean:
	rm -f unit-tests.out dev.out
