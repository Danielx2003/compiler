#pragma once

#include <stddef.h>

#define SCOPE_STACK_SIZE 16

struct scope_t {
  size_t num_lines;
  int num_tokens;
};

struct scope_stack_t {
  struct scope_t *scopes;
  int top;
  int num_scopes;
  void (*peek)(struct scope_stack_t *, struct scope_t *);
  void (*create)(struct scope_stack_t *);
  void (*pop)(struct scope_stack_t *, struct scope_t *);
};

void peek_scope(struct scope_stack_t *stack, struct scope_t *out_scope);
void pop_scope(struct scope_stack_t *stack, struct scope_t *out_scope);
void create_scope(struct scope_stack_t *stack);

void init_scope_stack(struct scope_stack_t *stack);

void scope_inc_num_lines(struct scope_stack_t *stack);
void scope_inc_num_tokens(struct scope_stack_t *stack);

extern struct scope_stack_t scope_stack;
 
