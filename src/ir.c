#include "ir.h"

#include <string.h>
#include <stdio.h>

static int temp_count = 0;
static int label_count = 0;

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

char *op_to_text(enum ast_operator_type op)
{
  switch (op)
  {
    case AST_OPERATOR_TYPE_ADD:
      return "+";
    case AST_OPERATOR_TYPE_SUBTRACT:
      return "-";
    case AST_OPERATOR_TYPE_EQUIV:
      return "==";
    case AST_OPERATOR_TYPE_LESS_THAN:
      return "<";
    case AST_OPERATOR_TYPE_GREATER_THAN:
      return ">";
    default:
      return "_";
  }
}

void create_if(int temp, int label)
{
  printf("ifz t%d goto L%d\n", temp, label);
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
  ret.temp = ++temp_count;

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
  ret.temp = ++temp_count;
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
  ret.temp = ++temp_count;
  struct ir_ret expr_ret = ir_expr(&assign->expr);
  printf("%s = ", assign->term.id.text);
  print_ir_ret(&expr_ret);
  printf("\n");
}

struct ir_ret ir_condition(struct ast_condition *cond)
{
  struct ir_ret ret;
  printf("t%d = %s %s %s \n", 
      ++temp_count,
      cond->left_term.id.text,
      op_to_text(cond->op),
      cond->right_term.id.text
  );

  ret.type = IR_RET_TYPE_TEMP;
  ret.temp = temp_count;
  return ret;
}

void ir_condition_body(struct ast_condition_body *body)
{
  for (int i = 0; i < body->num_lines; i++)
  {
    ir_line(&body->lines[i]);
  }
}

void ir_conditional(struct ast_conditional *cond)
{
  struct ir_ret condition_ret = ir_condition(&cond->condition);
  int local_label = ++label_count;
  create_if(condition_ret.temp, local_label);
  // If Block
  ir_condition_body(&cond->body);
  // goto local_label
  // Else Block
  printf("L%d:\n", local_label);
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
