#pragma once

#include "parse.h"

enum ir_ret_type {
  IR_RET_TYPE_TEMP,
  IR_RET_TYPE_ID,
  IR_RET_TYPE_CONSTANT,
  IR_RET_TYPE_FUNC_CALL,
  IR_RET_TYPE_NULL
};

struct ir_func_call {
  char text[32]; // name of function
  int return_temp;
  struct ir_term *args;
  int num_args;
};

struct ir_ret {
  enum ir_ret_type type;
  union {
    int temp;
    int constant;
    char text[32];
    struct ir_func_call func_call;
  };
};

enum ir_cond_op {
  IR_COND_OPERATOR_TYPE_EQUIV,
  IR_COND_OPERATOR_TYPE_LESS_THAN,
  IR_COND_OPERATOR_TYPE_GREATER_THAN,
};

enum ir_op {
  IR_ASSIGN_OP_NONE,
  IR_ASSIGN_OP_ADD,
  IR_ASSIGN_OP_SUBTRACT,
  IR_ASSIGN_OP_EQUIV,
  IR_ASSIGN_OP_LESS_THAN,
  IR_ASSIGN_OP_GREATER_THAN
};

enum ir_assign_type {
  IR_ASSIGN_VALUE,
  IR_ASSIGN_CONDITION
};

enum ir_term_type {
  IR_TERM_NULL,
  IR_TERM_TEMP,
  IR_TERM_ID,
  IR_TERM_CONSTANT,
  IR_TERM_FUNC_CALL
};

struct ir_term {
  enum ir_term_type type;
  union {
    int temp;
    int constant;
    char text[32];
    struct ir_func_call func_call;
  };
};

struct ir_assign {
  struct ir_term lhs;
  struct ir_term rhs_1;
  struct ir_term rhs_2;
  enum ir_op op;
};

struct ir_conditional {
  int temp;
  int label;
};

struct ir_label {
  int label;
};

struct ir_goto {
  int label;
};

struct ir_param {
  char text[32];
};

struct ir_params {
  struct ir_param *params;
  int total;
};

struct ir_function_def {
  char name[32];
};

enum ir_item_type {
  IR_ITEM_ASSIGN,
  IR_ITEM_CONDITIONAL,
  IR_ITEM_LABEL,
  IR_ITEM_GOTO,
  IR_ITEM_FUNC_DECL,
  IR_ITEM_FUNC_PARAMS,
  IR_ITEM_FUNC_CALL,
  IR_ITEM_FUNC_RET
};

struct ir_item {
  enum ir_item_type type;
  union {
    struct ir_conditional conditional;
    struct ir_assign assign;
    struct ir_label label;
    struct ir_goto go_to;
    struct ir_function_def function_def;
    struct ir_params function_params;
    struct ir_func_call func_call;
  };
};

struct ir_stream {
  struct ir_item *items;
  int cur_idx;
  int total;
};

void ir_ast(struct ast_body *root, struct ir_stream *stream);
void ir_condition_body(struct ast_body *body);
struct ir_ret ir_condition(struct ast_condition *cond);
struct ir_ret ir_assignment(struct ast_assignment *assign);
struct ir_ret ir_expr(struct ast_expr *expr);
struct ir_ret ir_expr_prime(struct ast_expr_tail *tail);
void ir_line(struct ast_line *line);

void add_to_ir_list(struct ir_item *item);
enum ir_op ast_op_to_ir(enum ast_op_type ast_op);
