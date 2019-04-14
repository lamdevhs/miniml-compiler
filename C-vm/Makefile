all: core

core: main.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o core.out $^ && read && ./core.out

test: test.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o test.out $^ && read && ./test.out

testv: test.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DVERBOSE -o test.out $^ && read && ./test.out

gen: generated.c runtime.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o generated.out $^ && read && ./generated.out

gendbg: generated.c runtime.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DDEBUGMODE -o generated.out $^ && read && ./generated.out

.PHONY: clean

clean: 
	rm -f test.out core.out generated.out
