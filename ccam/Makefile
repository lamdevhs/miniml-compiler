all: core

core: main.c stack.c machine.c value.c ccam.h
	gcc -Wall -DTRACE_MEMORY -o core.out $^ && read PAUSING && ./core.out

test: unit-tests.c stack.c machine.c value.c ccam.h
	gcc -Wall -o unit-tests.out $^ && read PAUSING && ./unit-tests.out

testv: unit-tests.c stack.c machine.c value.c ccam.h
	gcc -Wall -DVERBOSE -o unit-tests.out $^ && read PAUSING && ./unit-tests.out

gen: $(name).c runtime.c stack.c machine.c value.c ccam.h
	gcc -Wall -o $(name).out $^ && read PAUSING && ./$(name).out

gendbg: $(name).c runtime.c stack.c machine.c value.c ccam.h
	gcc -Wall -DTRACE_MEMORY -DDEBUGMODE -o $(name).out $^ && read PAUSING && ./$(name).out

.PHONY: clean

clean:
	rm -f unit-tests.out core.out
