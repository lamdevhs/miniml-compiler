# usage: if you have generated a "gen.c" file with the compiler written in caml:
#	(normal mode)  $ make build name="gen"
# (debug mode)   $ make build name="gen" DBG=y
# (memory trace) $ make build name="gen" MEM=y
# and you can combine DBG=y and MEM=y together too.
build: $(name).c runtime.c stack.c machine.c value.c ccam.h
	gcc -Wall $(if $(MEM),-DTRACE_MEMORY,) $(if $(DBG),-DDEBUGMODE,) \
	  -o $(name).out $^

# builds then runs a miniml program on top of the CCAM (CAM written in C).
# usage: same as for 'make build'.
run: build
		read PRESS_ENTER_TO_CONTINUE && ./$(name).out

# launch the unit tests for the CCAM. usage:
# (silent)  $ make tests
# (verbose) $ make tests V=y
tests: unit-tests.c stack.c machine.c value.c ccam.h
	gcc -Wall $(if $(V),-DVERBOSE,) -o unit-tests.out $^ \
	  && read PRESS_ENTER_TO_CONTINUE \
		&& ./unit-tests.out

.PHONY: clean

clean:
	rm -f unit-tests.out
