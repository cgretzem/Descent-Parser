CC=g++
CFLAGS = -g -I.

DEPS = lexer.h parser.h inputbuf.h typer.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

makeparser: parser.o inputbuf.o lexer.o expression.o typer.o variable.o
	$(CC) -o parser parser.o inputbuf.o lexer.o expression.o typer.o variable.o