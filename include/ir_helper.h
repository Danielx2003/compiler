#pragma once

#include "ir.h"

char *ir_op_to_text(enum ir_cond_op op);


void print_ir_term(struct ir_term *term);
void print_ir_op(enum ir_op op);
void print_ir_assign(struct ir_assign *assign);
void print_ir_label(struct ir_label *label);
void print_ir_conditional(struct ir_conditional *cond);
void print_ir_item(struct ir_item *item);
