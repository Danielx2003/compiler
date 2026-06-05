#include "ir.h"

#include <string.h>
#include <stdio.h>

static int temp_count = 0;
static int label_count = 0;

struct ir_item ir_list[32] = {0};
static int ir_idx = 0;

struct ir_item ir_create_label(int label)
{
  printf("L%d:", label);
  return (struct ir_item){
    .type = IR_ITEM_LABEL,
    .label = {
      .label = label
    }
  };
}

void ir_set_term_from_ast(struct ir_term *ir, struct ast_term *ast)
{
  if (ast->type == AST_TERM_ID)
  {
    ir->type = IR_TERM_ID;
    strcpy(ir->text, ast->id.text);
    printf("%s", ast->id.text);
  }
  else if (ast->type == AST_TERM_CONSTANT)
  {
    ir->type = IR_TERM_CONSTANT;
    ir->constant = ast->constant.value;
    printf("%d", ast->constant.value);
  }
  else
  {
    printf("other\n");
  }
}

void ir_set_term_from_ret(struct ir_term *ir, struct ir_ret *ret)
{
  if (ret->type == IR_RET_TYPE_TEMP)
  {
    ir->temp = ret->temp;
    printf("t%d", ir->temp);
  }
  else if (ret->type == IR_RET_TYPE_ID)
  {
    strcpy(ir->text, ret->text);
    printf("%s", ir->text);
  }
  else if (ret->type == IR_RET_TYPE_CONSTANT)
  {
    ir->constant = ret->constant;
    printf("%d", ir->constant);
  }
}

void ret_set_term_from_ast(struct ir_ret *ret, struct ast_term *ast)
{
  if (ast->type == AST_TERM_ID)
  {
    ret->type = IR_RET_TYPE_ID;
    strcpy(ret->text, ast->id.text);
  }
  else if (ast->type == AST_TERM_CONSTANT)
  {
    ret->type = IR_RET_TYPE_CONSTANT;
    ret->constant = ast->constant.value;
  }
  else
  {
    printf("other\n");
  }
}


void print_ir_ret(struct ir_ret *ret)
{
  if (ret->type == IR_RET_TYPE_TEMP)
  {
    printf("t%d", ret->temp);
  }
  else
  { 
    printf("%s", ret->text);
  }
}

char *ir_op_to_text(enum ir_cond_op op)
{
  switch (op)
  {
    case IR_COND_OPERATOR_TYPE_EQUIV:
      return "==";
    case IR_COND_OPERATOR_TYPE_LESS_THAN:
      return "<";
    case IR_COND_OPERATOR_TYPE_GREATER_THAN:
      return ">";
  }
}

char *ast_op_to_text(enum ast_op_type op)
{
  switch (op)
  {
    case AST_OP_ADD:
      return "+";
    case AST_OP_SUBTRACT:
      return "-";
    case AST_OP_EQUIV:
      return "==";
    case AST_OP_LESS_THAN:
      return "<";
    case AST_OP_GREATER_THAN:
      return ">";
    default:
      return "_";
  }
}

void add_to_ir_list(struct ir_item *item)
{
  memcpy(&ir_list[ir_idx], item, sizeof(struct ir_item));
  ir_idx++;
}

void ir_create_if(int temp, int label)
{
  struct ir_item item = {0};
  item.type = IR_ITEM_CONDITIONAL;
  item.conditional.temp = temp;
  item.conditional.label = label;
  add_to_ir_list(&item);

  printf("ifz t%d goto L%d\n", item.conditional.temp, item.conditional.label);
}

struct ir_ret ir_expr_tail(struct ast_expr_tail *tail)
{
  struct ir_ret ret = {
    .type = IR_RET_TYPE_NULL,
    .temp = 0
  };

  if (tail == NULL)
  {
    return ret;
  }

  struct ir_ret tail_ret = ir_expr_tail(tail->next);

  if (tail->next == NULL)
  {
    ret_set_term_from_ast(&ret, &tail->term);
    return ret;
  }

  ret.type = IR_RET_TYPE_TEMP;
  ret.temp = ++temp_count;
  
  struct ir_item item = {
    .type = IR_ITEM_ASSIGN
  };

  ir_set_term_from_ret(&item.assign.lhs, &ret);
  printf("=");
  ir_set_term_from_ast(&item.assign.rhs_1, &tail->term);

  if (tail_ret.type != IR_RET_TYPE_NULL)
  {
    printf("+");
    ir_set_term_from_ret(&item.assign.rhs_2, &tail_ret);
  }

  add_to_ir_list(&item);
  printf("\n");
  
  return ret;
}

struct ir_ret ir_expr(struct ast_expr *expr)
{
  struct ir_ret ret = {0};
  ret.temp = ++temp_count;
  ret.type = IR_RET_TYPE_TEMP;

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;

  if (expr->tail)
  {
    struct ir_ret tail_ret = ir_expr_tail(expr->tail);
    ir_set_term_from_ret(&item.assign.lhs, &ret);
    printf("=");
    ir_set_term_from_ast(&item.assign.rhs_1, &expr->term);
    printf("+");
    ir_set_term_from_ret(&item.assign.rhs_2, &tail_ret);

    add_to_ir_list(&item);
  }
  else
  {
    ret_set_term_from_ast(&ret, &expr->term);
  }
  printf("\n");

  return ret;
}

struct ir_ret ir_assignment(struct ast_assignment *assign)
{
  struct ir_ret expr_ret = ir_expr(&assign->expr);

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;
  
  ir_set_term_from_ast(&item.assign.lhs, &assign->term);
  printf("=");
  ir_set_term_from_ret(&item.assign.rhs_1, &expr_ret);

  printf("\n");
  return (struct ir_ret) {0};
}

enum ir_cond_op ast_op_to_ir(enum ast_op_type ast_op)
{
  switch(ast_op)
  {
    case AST_OP_EQUIV:
      return IR_COND_OPERATOR_TYPE_EQUIV;
    case AST_OP_LESS_THAN:
      return IR_COND_OPERATOR_TYPE_LESS_THAN;
    case AST_OP_GREATER_THAN:
      return IR_COND_OPERATOR_TYPE_GREATER_THAN;
    default:
      printf("Invalid AST->IR map\n");
  }
}

struct ir_ret ir_condition(struct ast_condition *cond)
{
  int temp_local = ++temp_count;
  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;

  item.assign.lhs.temp = temp_local;
  printf("t%d = ", temp_local);
  ir_set_term_from_ast(&item.assign.rhs_1, &cond->left_term);
  printf("%s", ast_op_to_text(cond->op));
  ir_set_term_from_ast(&item.assign.rhs_2, &cond->right_term);

  printf("\n");

  return (struct ir_ret) {
    .type = IR_RET_TYPE_TEMP,
    .temp = temp_local
  };
}

void ir_condition_body(struct ast_body *body)
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
  ir_create_if(condition_ret.temp, local_label);

  // if
  ir_condition_body(&cond->body);
  ir_create_label(local_label);

  // else - not in the language yet
}

void ir_line(struct ast_line *line)
{
  switch(line->type)
  {
    case AST_LINE_CONDITIONAL:
      ir_conditional(&line->conditional);
      break;
    case AST_LINE_ASSIGNMENT:
      ir_assignment(&line->assignment);
      break;
  }
}

void print_ir_label(struct ir_label *label)
{
  printf("L%d:\n", label->label);
}

void print_ir_conditional(struct ir_conditional *cond)
{
  printf("ifz t%d goto L%d\n", cond->temp, cond->label);
}

void ir_ast(struct ast_body *root)
{
  // printf("%d Lines: \n", root->num_lines);
  for (int i=0; i<root->num_lines; i++)
  {
    ir_line(&root->lines[i]);
  }
}
