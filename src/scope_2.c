#include "scope_2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void peek_scope(struct scope_stack_t *stack, struct scope_t *out_scope)
{
  memcpy(out_scope, &stack->scopes[stack->top], sizeof(struct scope_t));
}

void pop_scope(struct scope_stack_t *stack, struct scope_t *out_scope)
{
  if (stack->top == 0)
  {
    return;
  }
  peek_scope(stack, out_scope);


  stack->top--;
  stack->scopes[stack->top].num_lines++;
  stack->scopes[stack->top].num_tokens += out_scope->num_tokens;
}

void create_scope(struct scope_stack_t *stack)
{
  if (stack->top+1 >= stack->num_scopes)
  {
    stack->scopes = (struct scope_t *)realloc(stack->scopes, sizeof(struct scope_t) * stack->num_scopes*2);

    if (stack->scopes == NULL)
    {
      printf("Failed to alloc new scopes\n");
      return;
    }

    stack->num_scopes = stack->num_scopes * 2;
  }

  stack->top++;
  memset(&stack->scopes[stack->top], 0, sizeof(struct scope_t));
}

void init_scope_stack(struct scope_stack_t *stack)
{
  stack->peek = peek_scope;
  stack->pop = pop_scope;
  stack->create = create_scope;
  stack->scopes = (struct scope_t *)calloc(SCOPE_STACK_SIZE, sizeof(struct scope_t));
  stack->num_scopes = SCOPE_STACK_SIZE;
  stack->top = 0;
}

void scope_inc_num_lines(struct scope_stack_t *stack)
{
  /*
  struct scope_t scope = {0};
  peek_scope(stack, &scope);
  scope.num_lines++;
  */
  stack->scopes[stack->top].num_lines++;
}


void scope_inc_num_tokens(struct scope_stack_t *stack)
{
  /*
  struct scope_t scope = {0};
  peek_scope(stack, &scope);
  scope.num_tokens++;
  */
  stack->scopes[stack->top].num_tokens++;
}

