#pragma once

#include "parse.h"

#include <stdbool.h>

struct symbol_table_t {
  char *symbols[8];
  int num_symbols;
  int top;
};

struct symbol_table_stack_t {
  struct symbol_table_t *stack;
  int top;
  int size;
};



void scope_root(struct symbol_table_stack_t *stack, struct ast_root *root);
void scope_line(struct symbol_table_stack_t *stack, struct ast_line *line);
void scope_assignment(struct symbol_table_stack_t *stack, struct ast_assignment *assign);
void scope_conditional(struct symbol_table_stack_t *stack, struct ast_conditional *conditional);
void scope_term(struct symbol_table_stack_t *stack, struct ast_term *term);
void scope_expr(struct symbol_table_stack_t *stack, struct ast_expr *expr);
void scope_body(struct symbol_table_stack_t *stack, struct ast_condition_body *body);
void scope_expr_prime(struct symbol_table_stack_t *stack, struct ast_expr_prime *expr_prime);


/*
linked list of scopes
each scope points back to the parent
the parent's parent ptr is NULL
if we cannot find a defined variable in a scope, then we check the parents
if we then can't find it at all, emit an error
 */

// Would use a hash table here, but a simple array will suffice for now
struct symbol_tbl {
  char* array[16]; // stores 16 names of variables
};

struct scope {
  struct symbol_tbl symbols;
  struct scope *parent;
};

bool scope_res(struct ast_root *root);


void print_root(struct ast_root *root);
void print_line(struct ast_line *line);
void print_assignment(struct ast_assignment *assign);
void print_conditional(struct ast_conditional *conditional);
void print_term(struct ast_term *term);
void print_expr(struct ast_expr *expr);
void print_body(struct ast_condition_body *body);
void print_expr_prime(struct ast_expr_prime *expr_prime);

