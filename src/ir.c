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

struct ir_ret ir_condition(struct ast_condition *cond)
{
  struct ir_ret;
  printf("t%d = %*s == %s \n", 
      temp_count++, 
      cond->left_term.id.text_len, cond->left_term.id.text, 
      cond->op,
      cond->right_term.id.text
  );
}

void ir_conditional(struct ast_conditional *cond)
{
  /*
   int x = 5
   if (x == 5)
   {
    int y = 5;
   }

  TAC:
   x = 5
   t1 = x == 5
   ifZ t1 goto _L0
   ret
 _L0:
    y = 5
   */ 

  struct ir_ret condition_ret = ir_condition(&cond->condition);
}

void ir_line(struct ast_line *line)
{
  switch(line->type)
  {
    case AST_LINE_CONDITIONAL:
      ir_conditional(&line->ctx.conditional);
      break;
    case AST_LINE_ASSIGNMENT:
      ir_assignment(&line->ctx.assignment);
      break;
  }
}

void ir_ast(struct ast_root *root)
{
  printf("%d Lines: \n", root->num_lines);
  for (int i=0; i<root->num_lines; i++)
  {
    ir_line(&root->lines[i]);
  }
}
