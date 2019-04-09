all: test

test: test.c
	gcc -Wall -o test.crap $< && ./test.crap

.PHONY: clean

clean: 
	rm -f test.crap
