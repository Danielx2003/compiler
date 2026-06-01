CC = gcc
CFLAGS = -I../include

compiler: main.c parse.c lexer.c scope.c ir.c
	$(CC) $(CFLAGS) main.c parse.c lexer.c scope.c ir.c -o ./build/main
