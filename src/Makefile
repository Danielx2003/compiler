CC = gcc
CFLAGS = -I../include

compiler: main.c parse.c lexer.c scope.c ir.c scope_2.c tokenize.c file_stream.c ir_helper.c ast_helper.c codegen.c
	$(CC) $(CFLAGS) main.c parse.c lexer.c scope.c ir.c scope_2.c tokenize.c file_stream.c ir_helper.c ast_helper.c codegen.c -o ./build/main
