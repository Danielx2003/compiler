#include "scope.h"
#include "parse.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int indent = 0;
bool scope_error = false;

bool is_symbol_table_full(struct symbol_table_t *table)
{
  if (table->cur_idx+1 >= table->capacity)
  {
    return true;
  }

  return false;
}

bool initialise_symbol_table(struct symbol_table_t *table)
{
  table->symbols = (char **)malloc(8 * sizeof(char *));

  for (int i = 0; i < 8; i++)
  {
    table->symbols[i] = (char *)malloc(sizeof(char) * 32);
  }

  table->capacity = 8;
  return true;
}

bool initialise_symbol_table_stack(struct symbol_table_stack_t *stack)
{
  stack->stack = (struct symbol_table_t *)calloc(8, sizeof(struct symbol_table_t));
  if (stack->stack == NULL) { return false; }
  stack->capacity = 8;

  for (int i = 0; i < 8; i++)
  {
    if (!initialise_symbol_table(&stack->stack[i])) { return false; }
  }
  return true;
}

void set_symbol_table_size(struct symbol_table_t *table, int size)
{
  int old_size = table->capacity;
  
  char **temp = (char **)realloc(table->symbols, size * sizeof(char *));

  if (temp == NULL)
  {
    printf("failed to alloc\n");
    return;
  }

  table->symbols = temp;

  for (int i = old_size; i < size; i++)
  {
    // Each symbol is at most 32 bytes long
    table->symbols[i] = (char *)malloc(sizeof(char) * 32);
  }

  table->capacity = size;
}

void set_symbol_table_stack_size(struct symbol_table_stack_t *stack, int size)
{
 stack->stack = realloc(stack->stack, sizeof(struct symbol_table_t) * size);
}

void add_id_to_scope(struct symbol_table_stack_t *stack, struct ast_id *id)
{
  if (is_symbol_table_full(&stack->stack[stack->cur_idx]))
  {
    set_symbol_table_size(&stack->stack[stack->cur_idx], stack->stack[stack->cur_idx].capacity * 2);
  }

  if (scope_lookup(stack, id->text))
  {
    printf("This term has already been added. Double Definition\n");
  }
  strcpy(stack->stack[stack->cur_idx].symbols[stack->stack[stack->cur_idx].cur_idx], id->text);
  stack->stack[stack->cur_idx].cur_idx++;
}

void add_term_to_scope(struct symbol_table_stack_t *stack, struct ast_term *term)
{
  if (term->type == AST_TERM_CONSTANT)
  {
    return;
  }

  if (is_symbol_table_full(&stack->stack[stack->cur_idx]))
  {
    set_symbol_table_size(&stack->stack[stack->cur_idx], stack->stack[stack->cur_idx].capacity * 2);
  }

  if (term->type == AST_TERM_ID)
  {
    if (scope_lookup(stack, term->id.text))
    {
      printf("This term has already been added. Double Definition\n");
    }

    strcpy(stack->stack[stack->cur_idx].symbols[stack->stack[stack->cur_idx].cur_idx], term->id.text);
  }
  else if (term->type == AST_TERM_FUNC_CALL)
  {
    if (scope_lookup(stack, term->func_call.id.text))
    {
      printf("This term has already been added. Double Definition\n");
    }

    strcpy(stack->stack[stack->cur_idx].symbols[stack->stack[stack->cur_idx].cur_idx], term->func_call.id.text);
  }

  stack->stack[stack->cur_idx].cur_idx++;
}

bool scope_lookup(struct symbol_table_stack_t *stack, char *str)
{
  for (int i = stack->cur_idx; i >= 0; i--)
  {
    for (int j = stack->stack[i].cur_idx; j >= 0; j--)
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
  if (stack->cur_idx+1 >= stack->capacity)
  {
    set_symbol_table_stack_size(stack, stack->capacity* 2);
  }
  stack->cur_idx++;
}

void pop_symbol_table_stack(struct symbol_table_stack_t *stack)
{
  if (stack->cur_idx== 0)
  {
    return;
  }

  memset(&stack->stack[stack->cur_idx], 0, sizeof(struct symbol_table_t));
  stack->cur_idx--;
}

void scope_body(struct symbol_table_stack_t *stack, struct ast_body *body)
{
  for (int i = 0; i < body->num_lines; i++)
  {
    scope_line(stack, &body->lines[i]);
  }
}

void scope_id(struct symbol_table_stack_t *stack, struct ast_id *id)
{
  if (!scope_lookup(stack, id->text))
  {
    printf("Failed to resolve: %s\n", id->text);
    scope_error = true;
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
  else if (term->type == AST_TERM_FUNC_CALL)
  {
    if (!scope_lookup(stack, term->func_call.id.text))
    {
      printf("Failed to resolve: %s\n", term->func_call.id.text);
      scope_error = true;
    }

    int args_count = 0;
    for (struct ast_arg *ptr =term->func_call.args; ptr != NULL; ptr=ptr->next)
    {
      args_count++;
    }
  }
}

void scope_expr_prime(struct symbol_table_stack_t *stack, struct ast_expr_tail *tail)
{
  if (tail == NULL)
  {
    return;
  }

  scope_term(stack, &tail->term);
  scope_expr_prime(stack, tail->next);
}

void scope_expr(struct symbol_table_stack_t *stack, struct ast_expr *expr)
{
  scope_term(stack, &expr->term);
  scope_expr_prime(stack, expr->tail);
}

void scope_var_decl(struct symbol_table_stack_t *stack,struct ast_var_decl *decl)
{
  add_id_to_scope(stack, &decl->id);
  scope_expr(stack, &decl->expr);
}

void scope_assignment(struct symbol_table_stack_t *stack, struct ast_assignment *assign)
{
  scope_id(stack, &assign->id);
  scope_expr(stack, &assign->expr);
}

void scope_condition(struct symbol_table_stack_t *stack, struct ast_condition *condition)
{
  scope_term(stack, &condition->left_term);
  scope_term(stack, &condition->right_term);
}

void scope_if_stmt(struct symbol_table_stack_t *stack, struct ast_if_stmt *conditional)
{
  scope_condition(stack, &conditional->condition);
  create_symbol_table_stack(stack);
  scope_body(stack, &conditional->body);
  pop_symbol_table_stack(stack);
}

void scope_while_loop(struct symbol_table_stack_t *stack, struct ast_while_loop *conditional)
{
  scope_condition(stack, &conditional->condition);
  create_symbol_table_stack(stack);
  scope_body(stack, &conditional->body);
  pop_symbol_table_stack(stack);
}

void scope_func_call(struct symbol_table_stack_t *stack, struct ast_func_call *func)
{
  printf("scope func call\n");
}

void scope_func_decl(struct symbol_table_stack_t *stack, struct ast_func_decl *func)
{
  // add_id_to_scope(stack, &func->id);

  for (struct ast_param *ptr = func->params; ptr!= NULL; ptr = ptr->next)
  {
    struct ast_term temp = {
      .type = AST_TERM_ID
    };
    strcpy(temp.id.text, ptr->id.text);
    add_term_to_scope(stack, &temp);
  }

  add_id_to_scope(stack, &func->id);
  scope_body(stack, &func->body);
}

void scope_line(struct symbol_table_stack_t *stack, struct ast_line *line)
{
  if (line->type == AST_LINE_ASSIGNMENT)
  {
    scope_assignment(stack, &line->assignment);
  }
  else if (line->type == AST_LINE_IF_STMT)
  {
    scope_if_stmt(stack, &line->if_stmt);
  }
  else if (line->type == AST_LINE_WHILE_LOOP)
  {
    scope_while_loop(stack, &line->while_loop);
  }
  else if (line->type == AST_LINE_VAR_DECL)
  {
    scope_var_decl(stack, &line->var_decl);
  }
  else if (line->type == AST_LINE_FUNC_DECL)
  {
    scope_func_decl(stack, &line->func_decl);
  }
  else if (line->type == AST_LINE_FUNC_CALL)
  {
    scope_func_call(stack, &line->func_call);
  }
  else 
  {
    printf("could not match to a line type\n");
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
