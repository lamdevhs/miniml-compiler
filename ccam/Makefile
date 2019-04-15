all: core

core: main.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o core.out $^ && read && ./core.out

test: unit-tests.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o unit-tests.out $^ && read && ./unit-tests.out

testv: unit-tests.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DVERBOSE -o unit-tests.out $^ && read && ./unit-tests.out

gen: generated.c runtime.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o generated.out $^ && read && ./generated.out

gendbg: generated.c runtime.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DDEBUGMODE -o generated.out $^ && read && ./generated.out

.PHONY: clean

clean: 
	rm -f unit-tests.out core.out generated.out
