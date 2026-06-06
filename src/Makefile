CC = gcc
CFLAGS = -I../include

compiler: main.c parse.c lexer.c scope.c ir.c scope_2.c tokenize.c file_stream.c ir_helper.c ast_helper.c codegen.c cfg.c
	$(CC) $(CFLAGS) main.c parse.c lexer.c scope.c ir.c scope_2.c tokenize.c file_stream.c ir_helper.c ast_helper.c codegen.c cfg.c -o ./build/main
