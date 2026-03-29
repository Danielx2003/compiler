#pragma once

#include "lexer.h" // Migrate tokens to own header file

enum ast_node_type {
  AST_NODE_TYPE_OPERATOR,
  AST_NODE_TYPE_ID,
  AST_NODE_TYPE_CONSTANT
};

enum ast_expr_prime_type {
  AST_EXPR_PRIME_TYPE_EMPTY,
  AST_EXPR_PRIME_TYPE_EXPR
};

enum ast_operator_type {
  AST_OPERATOR_TYPE_ADD,
  AST_OPERATOR_TYPE_SUBTRACT
};

struct ast_operator {
  enum ast_operator_type type;
};


enum ast_term_type {
  AST_TERM_TYPE_ID,
  AST_TERM_TYPE_CONSTANT
};

// Maybe merge id and constant and differentiate by type enum
struct ast_id {
  char text[32];
  size_t text_len;
};

struct ast_constant {
  char text[32];
  size_t text_len;
};

struct ast_term {
  enum ast_term_type type;
  union {
    struct ast_id id;
    struct ast_constant constant;
  };
};

struct ast_expr_prime {
  // Could be worth converting the grammar for op, term, expr' to be a single non-terminal
  enum ast_expr_prime_type type; // Could remove and use NULL for expr_prime_ptr
  struct ast_operator op; // Could migrate to enum, however for future changes struct is better
  struct ast_term term;
  struct ast_expr_prime *expr_prime;
};

struct ast_expr {
  struct ast_term term;
  struct ast_expr_prime expr_prime;
};

enum ast_type {
  // Will introduce family, and other things later -> make it a struct
  AST_TYPE_INT 
};

struct ast_assignment {
  enum ast_type type;
  struct ast_term term;
  struct ast_expr expr;
};

struct ast_line {
  struct ast_assignment assignment;
  // Don't see a need for terminator here
};

struct ast_root {
  struct ast_line *lines;
  size_t num_lines;
};

void free_ast(struct ast_root *root);

void parse_lexer_tokens(
  struct token_list_t *lexer_output,
  int nun_lines
);

void peek_token(
    struct token_list_t *lexer_output
);

void consume_token(struct token_list_t *lexer_output);
