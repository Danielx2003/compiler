#include "scope.h"
#include "parse.h"

#include <stdio.h>

void scope_body(struct ast_condition_body *body)
{
  scope_assignment(&body->assignment);
}

void scope_term(struct ast_term *term)
{
  if (term->type == AST_TERM_TYPE_ID)
  {
    printf("Resolve: %s\n", term->id.text);
  }
}

void scope_expr_prime(struct ast_expr_prime *expr_prime)
{
  if (expr_prime->expr_prime == NULL || expr_prime->type == AST_EXPR_PRIME_TYPE_NULL)
  {
    return;
  }

  if (expr_prime->type == AST_EXPR_PRIME_TYPE_TERM_ONLY)
  {
    scope_term(&expr_prime->term);
    return;
  }

  scope_expr_prime(expr_prime->expr_prime);
}

void scope_expr(struct ast_expr *expr)
{
  scope_term(&expr->term);
  scope_expr_prime(&expr->expr_prime);
}

void scope_assignment(struct ast_assignment *assign)
{
  scope_term(&assign->term);
  scope_expr(&assign->expr);
}

void scope_conditional(struct ast_conditional *conditional)
{
  printf("--- start scope ---\n");
  scope_body(&conditional->body);
  printf("--- end scope ---\n");
}

void scope_line(struct ast_line *line)
{
  if (line->type == AST_LINE_ASSIGNMENT)
  {
    scope_assignment(&line->ctx.assignment);
  }
  else if (line->type == AST_LINE_CONDITIONAL)
  {
    scope_conditional(&line->ctx.conditional);
  }
}

void scope_root(struct ast_root *root)
{
  for (int i = 0; i < root->num_lines; i++)
  {
    scope_line(&root->lines[i]);
  }
}

void scope_res(struct ast_root *root)
{
  printf("--- start scope ---\n");
  scope_root(root);
  printf("--- end scope ---\n");
}
