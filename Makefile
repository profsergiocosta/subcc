# Compiler
CC=gcc
# Parameters given to the compiler
CFLAGS=-Wno-deprecated
OUTPUT="subc"

# Source files
SRCS="interprete.c" "lexical.c" "main.c" "subc.c" "symbol.c" "mstring.c" "error.c"
# Output object files (*.o)
OBJS="interprete.o" "lexical.o" "main.o" "subc.o" "symbol.o" "mstring.o" "error.o"  

# Standard targets

all: 
	$(CC) -c $(SRCS) $(CFLAGS)
	$(CC) -o $(OUTPUT) $(OBJS) $(CFLAGS)


clean:
	rm *.o

mrproper: clean

