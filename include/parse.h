#pragma once

#include "lexer.h" // Migrate tokens to own header file
#include "ast_type.h"

struct ast_operator {
  enum ast_op_type type;
};

struct ast_id {
  char text[32];
  size_t text_len;
};

struct ast_constant {
  int value;
};

struct ast_term {
  enum ast_term_type type;
  union {
    struct ast_id id;
    struct ast_constant constant;
  };
};

/*
struct ast_expr_prime {
  // Could be worth converting the grammar for op, term, expr' to be a single non-terminal
  enum ast_expr_prime_type type; // Could remove and use NULL for expr_prime_ptr
  struct ast_operator op; // Could migrate to enum, however for future changes struct is better
  struct ast_term term;
  struct ast_expr_prime *expr_prime;
};
*/

struct ast_expr_tail {
  enum ast_expr_prime_type type;
  enum ast_op_type op;
  struct ast_term term;
  struct ast_expr_tail *next;
};

struct ast_expr {
  struct ast_term term;
  struct ast_expr_tail tail;
};

/*
struct ast_expr {
  struct ast_term term;
  struct ast_expr_prime expr_prime;
};
*/


struct ast_assignment {
  enum ast_type type;
  struct ast_term term;
  struct ast_expr expr;
};

/*
struct ast_condition_body {
  struct ast_line *lines;
  int num_lines;
};
*/

struct ast_body {
  struct ast_line *lines;
  size_t num_lines;
};

struct ast_condition {
  struct ast_term left_term;
  enum ast_op_type op;
  struct ast_term right_term;
};

struct ast_conditional {
  struct ast_condition condition;
  struct ast_body body;
};

/*
enum ast_line_type {
  AST_LINE_ASSIGNMENT,
  AST_LINE_CONDITIONAL
};
*/

struct ast_line {
  enum ast_line_type type;
  union {
    struct ast_assignment assignment;
    struct ast_conditional conditional;
  };
};

void free_ast(struct ast_body *root);

struct ast_body* parse_lexer_tokens(
  struct lex_token_list_t *lexer_output,
  int nun_lines
);

void peek_token(
  struct lex_token_list_t *lexer_output
);

void consume_token(struct lex_token_list_t *lexer_output);

void parse_line(
  struct lex_token_list_t *lexer_output,
  struct ast_line *line
);
