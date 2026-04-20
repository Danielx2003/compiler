#pragma once

#include "parse.h"

void ir_ast(struct ast_root *root);

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
