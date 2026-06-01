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


/* IR Structs */

enum ir_cond_op {
  IR_COND_OPERATOR_TYPE_EQUIV,
  IR_COND_OPERATOR_TYPE_LESS_THAN,
  IR_COND_OPERATOR_TYPE_GREATER_THAN,
};

enum ir_assign_op {
  IR_ASSIGN_OPERATOR_TYPE_NONE,
  IR_ASSIGN_OPERATOR_TYPE_ADD,
  IR_ASSIGN_OPERATOR_TYPE_SUBTRACT
};

enum ir_condition_type {
  IR_CONDITION_TYPE_TEMP_TEMP,
  IR_CONDITION_TYPE_TEMP_TERM, 
  IR_CONDITION_TYPE_TERM_TEMP,
  IR_CONDITION_TYPE_TERM_TERM
};

struct ir_condition {
  enum ir_condition_type type;
  enum ir_cond_op op;
  union {
    struct {
      int temp_l;
      int temp_r;
    } temp_temp;
    struct {
      int temp;
      char term[32];
    } temp_term;
    struct {
      char term[32];
      int temp;
    } term_temp;
    struct {
      char term_l[32];
      char term_r[32];
    } term_term;
  };
};

enum ir_assign_type {
  IR_ASSIGN_VALUE,
  IR_ASSIGN_CONDITION
};

enum ir_assign_value_type {
  IR_ASSIGN_VALUE_SINGLE,
  IR_ASSIGN_VALUE_DOUBLE
};

struct ir_assign_value {
  enum ir_assign_value_type type;
  enum ir_assign_op assign_op;
  union {
    int temp;
    char term_lhs[32];
  } lhs;
  union {
    int term;
    char term_rhs[32];
  } rhs;
};

struct ir_assign {
  enum ir_assign_type type;
  union {
    struct ir_assign_value value;
    struct ir_condition condition;
  };
};

struct ir_conditional {
  int temp;
  int label;
};

struct ir_label {
  int label;
};

struct ir_item {
  union {
    struct ir_conditional conditional;
    struct ir_assign assign;
    struct ir_label label;
  };
};

/*
assign:
= num:
- temp = 
- term = 

= condtion:
 - temp = 
 - term = 


branch/conditional:
- temp
- goto label

label:
- label
 */


/*
x = 1
t3 = x == 5
ifz t3 goto L1
a = 1
t6 = x == 4 
ifz t6 goto L2
b = 2
L2:
c = 3
L1:
d = 4
*/


