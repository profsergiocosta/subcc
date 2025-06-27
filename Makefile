# Compiler
CC=gcc
# Parameters given to the compiler
CFLAGS=-Wno-deprecated
OUTPUT="subc"

# Source files
SRCS="interprete.c" "scanner.c" "parser.c" "subc.c" "symbol.c" "mstring.c" "error.c"
# Output object files (*.o)
OBJS="interprete.o" "scanner.o" "parser.o" "subc.o" "symbol.o" "mstring.o" "error.o"  

# Standard targets

all: 
	$(CC) -c $(SRCS) $(CFLAGS)
	$(CC) -o $(OUTPUT) $(OBJS) $(CFLAGS)


clean:
	rm *.o

mrproper: clean

