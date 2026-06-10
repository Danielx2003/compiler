#pragma once

#include "lexer.h" // Migrate tokens to own header file
#include "ast_type.h"

#include <stdbool.h>

/*

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
  enum ast_op_type op;
  struct ast_term term;
  struct ast_expr_tail *next;
};

struct ast_expr {
  struct ast_term term;
  struct ast_expr_tail *tail;
};

struct ast_declaration {
  enum ast_type type;
  struct ast_term term;
  struct ast_expr expr;
};

struct ast_assignment {
  // enum ast_type type;
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
  enum ast_conditional_type type;
  struct ast_condition condition;
  struct ast_body body;
};

struct ast_param {
  char text[32];
  struct ast_param *next;
};

struct ast_function_def {
  enum ast_type ret_type;
  struct ast_id name;
  struct ast_param *params;
  struct ast_body body;
};

struct ast_arg {
  enum ast_term_type type;
  union {
    int constant;
    char text[32];
  };
  struct ast_arg *next;
};

struct ast_function_call {
  struct ast_id name;
  struct ast_arg *args;
};

struct ast_line {
  enum ast_line_type type;
  union {
    struct ast_declaration declaration;
    struct ast_assignment assignment;
    struct ast_conditional conditional;
    struct ast_function_def function_def;
    struct ast_function_call function_call;
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

*/

struct ast_id {
  char text[32];
};

struct ast_func_call {
  struct ast_id id;
  struct ast_arg *args;
};

struct ast_term {
  enum ast_term_type type;
  union {
    struct ast_id id;
    struct ast_func_call func_call;
    int constant;
  };
};

struct ast_ret {
  struct ast_term term;
};

struct ast_param {
  enum ast_type type;
  struct ast_id id;
  struct ast_param *next;
};

struct ast_expr_tail {
  enum ast_op_type op;
  struct ast_term term;
  struct ast_expr_tail *next;
};

struct ast_expr {
  struct ast_term term;
  struct ast_expr_tail *tail;
};

struct ast_arg {
  struct ast_term term;
  struct ast_arg *next;
};

struct ast_var_decl {
  enum ast_type type;
  struct ast_id id;
  struct ast_expr expr;
};

struct ast_assignment {
  struct ast_id id;
  struct ast_expr expr;
};

struct ast_condition {
  struct ast_term left_term;
  struct ast_term right_term;
  enum ast_op_type op;
};

struct ast_body {
  struct ast_line *lines;
  int num_lines;
};

struct ast_while_loop {
  struct ast_condition condition;
  struct ast_body body;
};

struct ast_if_stmt {
  struct ast_condition condition;
  struct ast_body body;
};

struct ast_func_decl {
  enum ast_type return_type;
  struct ast_id id;
  struct ast_param *params;
  struct ast_body body;
};

struct ast_line {
  enum ast_line_type type;
  union {
    struct ast_assignment assignment;
    struct ast_if_stmt if_stmt;
    struct ast_while_loop while_loop;
    struct ast_var_decl var_decl;
    struct ast_func_decl func_decl;
    struct ast_func_call func_call;
    struct ast_ret ret;
  };
};

void free_ast(struct ast_body *root);

struct ast_body* parse_lexer_tokens(
  struct lex_token_stream *lexer_output,
  int nun_lines
);

struct lex_token_t parser_peek(struct lex_token_stream *lexer_output);
struct lex_token_t parser_peek_n(struct lex_token_stream *lexer_output, int n);
void parser_consume(struct lex_token_stream *tokens);
bool parser_match(struct lex_token_stream *tokens, enum lex_token_type expected_type);

void parse_line(
  struct lex_token_stream *lexer_output,
  struct ast_line *line
);

bool parse_body(
  struct lex_token_stream *tokens,
  struct ast_body *body
);


bool parse_params(struct lex_token_stream *tokens, struct ast_param **param);
bool parse_args(struct lex_token_stream *tokens, struct ast_arg **arg);
