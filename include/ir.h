#pragma once

#include "parse.h"

void ir_ast(struct ast_root *root);
void ir_conditional(struct ast_conditional *cond);
void ir_condition_body(struct ast_condition_body *body);
struct ir_ret ir_condition(struct ast_condition *cond);
struct ir_ret ir_assignment(struct ast_assignment *assign);
struct ir_ret ir_expr(struct ast_expr *expr);
struct ir_ret ir_expr_prime(struct ast_expr_prime *expr_prime);
void ir_line(struct ast_line *line);

enum ir_ret_type {
  IR_RET_TYPE_TEMP,
  IR_RET_TYPE_TERM,
  IR_RET_TYPE_NULL
};

struct ir_ret_id {
  char text[32];
  size_t text_len;
};

struct ir_ret {
  enum ir_ret_type type;
  union {
    int temp;
    struct ir_ret_id id;
  };
};
