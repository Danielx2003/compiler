#pragma once

#include "lexer.h" // Migrate tokens to own header file
#include "ast_type.h"

#include <stdbool.h>

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

struct ast_expr_tail {
  enum ast_expr_prime_type type;
  enum ast_op_type op;
  struct ast_term term;
  struct ast_expr_tail *next;
};

struct ast_expr {
  struct ast_term term;
  struct ast_expr_tail *tail;
};

struct ast_assignment {
  enum ast_type type;
  struct ast_term term;
  struct ast_expr expr;
};

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

struct ast_line {
  enum ast_line_type type;
  union {
    struct ast_assignment assignment;
    struct ast_conditional conditional;
  };
};

void free_ast(struct ast_body *root);

struct ast_body* parse_lexer_tokens(
  struct lex_token_stream *lexer_output,
  int nun_lines
);

struct lex_token_t parser_peek(struct lex_token_stream *lexer_output);
void parser_consume(struct lex_token_stream *tokens);
bool parser_match(struct lex_token_stream *tokens, enum lex_token_type expected_type);

void parse_line(
  struct lex_token_stream *lexer_output,
  struct ast_line *line
);
