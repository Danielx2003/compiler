#include "scope.h"
#include "parse.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int indent = 0;
bool scope_error = false;

void print_body(struct ast_body *body)
{
  for (int i = 0; i < body->num_lines; i++)
  {
    print_line(&body->lines[i]);
  }
}

void print_term(struct ast_term *term)
{
  printf("%s ", term->id.text);
}

void print_expr_prime(struct ast_expr_prime *expr_prime)
{
  if (expr_prime->expr_prime == NULL || expr_prime->type == AST_EXPR_PRIME_NULL)
  {
    return;
  }

  if (expr_prime->type == AST_EXPR_PRIME_TERM_ONLY)
  {
    print_term(&expr_prime->term);
    return;
  }

  print_expr_prime(expr_prime->expr_prime);
}

void print_expr(struct ast_expr *expr)
{
  print_term(&expr->term);
  print_expr_prime(&expr->expr_prime);
}


void print_assignment(struct ast_assignment *assign)
{
  for (int i = 0; i < indent; i++)
  {
    printf("-");
  }
  printf(" int ");
  print_term(&assign->term);
  printf(" = ");
  print_expr(&assign->expr);
  printf("\n");
}

void print_conditional(struct ast_conditional *conditional)
{
  indent += 4;
  print_body(&conditional->body);
  indent -= 4;
}

void print_line(struct ast_line *line)
{
  if (line->type == AST_LINE_ASSIGNMENT)
  {
    print_assignment(&line->assignment);
  }
  else if (line->type == AST_LINE_CONDITIONAL)
  {
    for (int i = 0; i < indent; i++)
    {
      printf("-");
    }
    printf("If Statement\n");
    print_conditional(&line->conditional);
    printf("\n");
  }
}

void print_root(struct ast_body *root)
{
  for (int i = 0; i < root->num_lines; i++)
  {
    print_line(&root->lines[i]);
  }
}

bool is_symbol_table_full(struct symbol_table_t *table)
{
  if (sizeof(table->symbols) == table->num_symbols)
  {
    return true;
  }

  return false;
}

bool initialise_symbol_table(struct symbol_table_t *table)
{
  for (int i = 0; i < 8; i++)
  {
    table->symbols[i] = (char *)calloc(1, sizeof(char));
    if (table->symbols[i] == NULL) { return false; }
  }

  return true;
}

bool initialise_symbol_table_stack(struct symbol_table_stack_t *stack)
{
  stack->stack = (struct symbol_table_t *)calloc(8, sizeof(struct symbol_table_t));
  if (stack->stack == NULL) { return false; }
  stack->size = 8;

  for (int i = 0; i < 8; i++)
  {
    if (!initialise_symbol_table(&stack->stack[i])) { return false; }
  }
  return true;
}

void set_symbol_table_size(struct symbol_table_t *table, int size)
{
  *table->symbols = (char *)realloc(table->symbols, size);
  if (table->symbols == NULL)
  {
    printf("failed to realloc");
  }
}

void set_symbol_table_stack_size(struct symbol_table_stack_t *stack, int size)
{
 stack->stack = realloc(stack->stack, sizeof(struct symbol_table_t) * size);
}

void add_to_scope(struct symbol_table_stack_t *stack, struct ast_term *term)
{
  if (term->type != AST_TERM_ID)
  {
    return;
  }

  if (is_symbol_table_full(&stack->stack[stack->top]))
  {
    set_symbol_table_size(&stack->stack[stack->top], stack->stack[stack->top].num_symbols * 2);
  }

  strcpy(stack->stack[stack->top].symbols[stack->stack[stack->top].top], term->id.text);
  stack->stack[stack->top].top++;
}

bool scope_lookup(struct symbol_table_stack_t *stack, char *str)
{
  for (int i = stack->top; i >= 0; i--)
  {
    for (int j = stack->stack[i].top; j >= 0; j--)
    {
      if (strcmp(str, stack->stack[i].symbols[j]) == 0)
      {
        return true;
      }
    }
  }
  return false;
}

void create_symbol_table_stack(struct symbol_table_stack_t *stack)
{
  if (stack->top+1 == stack->size)
  {
    set_symbol_table_stack_size(stack, stack->size * 2);
  }
  stack->top++;
}

void pop_symbol_table_stack(struct symbol_table_stack_t *stack)
{
  if (stack->top == 0)
  {
    return;
  }

  memset(&stack->stack[stack->top], 0, sizeof(struct symbol_table_t));
  stack->top--;
}

void scope_body(struct symbol_table_stack_t *stack, struct ast_body *body)
{
  for (int i = 0; i < body->num_lines; i++)
  {
    scope_line(stack, &body->lines[i]);
  }
}

void scope_term(struct symbol_table_stack_t *stack, struct ast_term *term)
{
  if (term->type == AST_TERM_ID)
  {
    if (!scope_lookup(stack, term->id.text))
    {
      printf("Failed to resolve: %s\n", term->id.text);
      scope_error = true;
    }
  }
}

void scope_expr_prime(struct symbol_table_stack_t *stack, struct ast_expr_prime *expr_prime)
{
  if (expr_prime->expr_prime == NULL || expr_prime->type == AST_EXPR_PRIME_NULL)
  {
    return;
  }

  if (expr_prime->type == AST_EXPR_PRIME_TERM_ONLY)
  {
    scope_term(stack, &expr_prime->term);
    return;
  }

  scope_expr_prime(stack, expr_prime->expr_prime);
}

void scope_expr(struct symbol_table_stack_t *stack, struct ast_expr *expr)
{
  scope_term(stack, &expr->term);
  scope_expr_prime(stack, &expr->expr_prime);
}

void scope_assignment(struct symbol_table_stack_t *stack, struct ast_assignment *assign)
{
  add_to_scope(stack, &assign->term);
  scope_expr(stack, &assign->expr);
}

void scope_condition(struct symbol_table_stack_t *stack, struct ast_condition *condition)
{
  scope_term(stack, &condition->left_term);
  scope_term(stack, &condition->right_term);
}

void scope_conditional(struct symbol_table_stack_t *stack, struct ast_conditional *conditional)
{
  scope_condition(stack, &conditional->condition);
  create_symbol_table_stack(stack);
  scope_body(stack, &conditional->body);
  pop_symbol_table_stack(stack);
}

void scope_line(struct symbol_table_stack_t *stack, struct ast_line *line)
{
  if (line->type == AST_LINE_ASSIGNMENT)
  {
    scope_assignment(stack, &line->assignment);
  }
  else if (line->type == AST_LINE_CONDITIONAL)
  {
    scope_conditional(stack, &line->conditional);
  }
}

void scope_root(struct symbol_table_stack_t *stack, struct ast_body *root)
{
  for (int i = 0; i < root->num_lines; i++)
  {
    scope_line(stack, &root->lines[i]);
  }
}

bool scope_res(struct ast_body *root)
{
  struct symbol_table_stack_t stack = {0};
  if (!initialise_symbol_table_stack(&stack))
  {
    printf("Failed to create symbol table.\n");
    return false;
  }

  // print_root(root);
  scope_root(&stack, root);
  return !scope_error; 
}
