#include "ir.h"

#include <string.h>
#include <stdio.h>

static int temp_count = 0;

// Using copy rather than ptrs from func return for simplicity

void print_ir_ret(struct ir_ret *ret)
{
  if (ret->type == IR_RET_TYPE_TEMP)
  {
    printf("t%d", ret->temp);
  }
  else
  { 
    printf("%s", ret->id.text);
  }
}

struct ir_ret ir_expr_prime(struct ast_expr_prime *expr_prime)
{
  struct ir_ret ret = {
    .type = IR_RET_TYPE_NULL
  };

  if (expr_prime == NULL || expr_prime->type == AST_EXPR_PRIME_TYPE_NULL)
  {
    return ret;
  }

  struct ir_ret expr_prime_ret = ir_expr_prime(expr_prime->expr_prime);
  ret.temp = temp_count++;

  if (expr_prime->type == AST_EXPR_PRIME_TYPE_TERM_ONLY)
  {
    ret.type = IR_RET_TYPE_TERM;
    memcpy(&ret.id, &expr_prime->term.id, sizeof(struct ast_id));
    return ret;
  }

  ret.type = IR_RET_TYPE_TEMP;
  
  if (expr_prime_ret.type == IR_RET_TYPE_NULL)
  {
    printf("t%d = %s", ret.temp, expr_prime->term.id.text);
  }
  else
  {
    printf("t%d = %s + ", ret.temp, expr_prime->term.id.text);
    print_ir_ret(&expr_prime_ret);
  }

  printf("\n");

  return ret;
}

struct ir_ret ir_expr(struct ast_expr *expr)
{
  struct ir_ret ret;
  ret.temp = temp_count++;
  ret.type = IR_RET_TYPE_TEMP;
  
  if (expr->expr_prime.type != AST_EXPR_PRIME_TYPE_NULL)
  {
    struct ir_ret expr_prime_ret = ir_expr_prime(&expr->expr_prime);
    printf("t%d = %s + ", ret.temp, expr->term.id.text);
    print_ir_ret(&expr_prime_ret);
    printf("\n");
  }
  else
  { 
    ret.type = IR_RET_TYPE_TERM;
    memcpy(&ret.id, &expr->term.id, sizeof(struct ast_id)); 
  }

  return ret;
}

struct ir_ret ir_assignment(struct ast_assignment *assign)
{
  struct ir_ret ret;
  ret.temp = temp_count++;
  struct ir_ret expr_ret = ir_expr(&assign->expr);
  printf("%s = ", assign->term.id.text);
  print_ir_ret(&expr_ret);
  printf("\n");
}

void ir_line(struct ast_line *line)
{
  // Once other line types are added, do a type check
  ir_assignment(&line->assignment);
}

void ir_ast(struct ast_root *root)
{
  for (int i=0; i<root->num_lines; i++)
  {
    ir_line(&root->lines[i]);
  }
}
