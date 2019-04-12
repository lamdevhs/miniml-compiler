all: core

core: main.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o core.crap $^ && read && ./core.crap

test: test.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o test.crap $^ && read && ./test.crap

testv: test.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DVERBOSE -o test.crap $^ && read && ./test.crap

.PHONY: clean

clean: 
	rm -f test.crap core.crap
