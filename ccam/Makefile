all: core

core: main.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o core.out $^ && read && ./core.out

test: unit-tests.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o unit-tests.out $^ && read && ./unit-tests.out

testv: unit-tests.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DVERBOSE -o unit-tests.out $^ && read && ./unit-tests.out

gen: $(name).c runtime.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -o $(name).out $^ && read && ./$(name).out

gendbg: $(name).c runtime.c stack.c machine.c value.c virtual-machine.h
	gcc -Wall -DDEBUGMODE -o $(name).out $^ && read && ./$(name).out

.PHONY: clean

clean: 
	rm -f unit-tests.out core.out
