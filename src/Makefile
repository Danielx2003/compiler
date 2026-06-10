CC = gcc
CFLAGS = -I../include

compiler: main.c parse.c lexer.c ir.c scope_2.c tokenize.c file_stream.c ast_helper.c scope.c codegen.c
	$(CC) $(CFLAGS) main.c parse.c lexer.c scope_2.c tokenize.c file_stream.c ast_helper.c scope.c ir.c ir_helper.c codegen.c -o ./build/main
