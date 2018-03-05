
CC=gcc
CFLAGS=-Wall
DEPS=Instr.h InstrList.h
OBJ=RegisterAllocator.o InstrList.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

alloc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f alloc $(OBJ)
