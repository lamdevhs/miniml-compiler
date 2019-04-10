all: test

test: main.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o test.crap $^ && ./test.crap

.PHONY: clean

clean: 
	rm -f test.crap
