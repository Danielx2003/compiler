#include "ir.h"
#include "ast_helper.h"
#include "ir_helper.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int temp_count = 0;
static int label_count = 0;

struct ir_item ir_list[32] = {0};
static int ir_idx = 0;

void ir_create_label(int label)
{
  struct ir_item item = {
    .type = IR_ITEM_LABEL,
    .label = {
      .label = label
    }
  };

  add_to_ir_list(&item);
}

void ir_create_goto(int label)
{
  struct ir_item item = {
    .type = IR_ITEM_GOTO,
    .go_to = {
      .label = label
    }
  };
  add_to_ir_list(&item);
}

void ir_set_term_from_ast(struct ir_term *ir, struct ast_term *ast)
{
  if (ast->type == AST_TERM_ID)
  {
    ir->type = IR_TERM_ID;
    strcpy(ir->text, ast->id.text);
  }
  else if (ast->type == AST_TERM_CONSTANT)
  {
    ir->type = IR_TERM_CONSTANT;
    ir->constant = ast->constant.value;
  }
}

void ir_set_term_from_ret(struct ir_term *ir, struct ir_ret *ret)
{
  if (ret->type == IR_RET_TYPE_TEMP)
  {
    ir->type = IR_TERM_TEMP;
    ir->temp = ret->temp;
  }
  else if (ret->type == IR_RET_TYPE_ID)
  {
    ir->type = IR_TERM_ID;
    strcpy(ir->text, ret->text);
  }
  else if (ret->type == IR_RET_TYPE_CONSTANT)
  {
    ir->type = IR_TERM_CONSTANT;
    ir->constant = ret->constant;
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
  ir_set_term_from_ast(&item.assign.rhs_1, &tail->term);

  if (tail_ret.type != IR_RET_TYPE_NULL)
  {
    item.assign.op = ast_op_to_ir(tail->op);
    ir_set_term_from_ret(&item.assign.rhs_2, &tail_ret);
  }

  add_to_ir_list(&item);
  
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
    ir_set_term_from_ast(&item.assign.rhs_1, &expr->term);
    ir_set_term_from_ret(&item.assign.rhs_2, &tail_ret);
    item.assign.op = ast_op_to_ir(expr->tail->op);

    add_to_ir_list(&item);
  }
  else
  {
    ret_set_term_from_ast(&ret, &expr->term);
  }

  return ret;
}

struct ir_ret ir_declaration(struct ast_declaration *decl)
{
  struct ir_ret expr_ret = ir_expr(&decl->expr);

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;
  
  ir_set_term_from_ast(&item.assign.lhs, &decl->term);
  ir_set_term_from_ret(&item.assign.rhs_1, &expr_ret);
  add_to_ir_list(&item);

  return (struct ir_ret) {0};
}

struct ir_ret ir_assignment(struct ast_assignment *assign)
{
  struct ir_ret expr_ret = ir_expr(&assign->expr);

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;
  
  ir_set_term_from_ast(&item.assign.lhs, &assign->term);
  ir_set_term_from_ret(&item.assign.rhs_1, &expr_ret);
  add_to_ir_list(&item);

  return (struct ir_ret) {0};
}

enum ir_op ast_op_to_ir(enum ast_op_type ast_op)
{
  switch(ast_op)
  {
    case AST_OP_EQUIV:
      return IR_ASSIGN_OP_EQUIV;
    case AST_OP_LESS_THAN:
      return IR_ASSIGN_OP_LESS_THAN;
    case AST_OP_GREATER_THAN:
      return IR_ASSIGN_OP_GREATER_THAN;
    case AST_OP_ADD:
      return IR_ASSIGN_OP_ADD;
    default:
      printf("Invalid AST->IR map. Type is: %d\n", ast_op);
  }
}

struct ir_ret ir_condition(struct ast_condition *cond)
{
  int temp_local = ++temp_count;
  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;

  item.assign.lhs.type = IR_TERM_TEMP;
  item.assign.lhs.temp = temp_local;
  ir_set_term_from_ast(&item.assign.rhs_1, &cond->left_term);
  item.assign.op = ast_op_to_ir(cond->op);
  ir_set_term_from_ast(&item.assign.rhs_2, &cond->right_term);

  add_to_ir_list(&item);

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
  int while_label = -1;
  if (cond->type == AST_CONDITIONAL_WHILE)
  {
    while_label = ++label_count;
    ir_create_label(while_label);
  }


  struct ir_ret condition_ret = ir_condition(&cond->condition);
  int local_label = ++label_count;
  ir_create_if(condition_ret.temp, local_label);


  ir_condition_body(&cond->body);
  
  if (cond->type == AST_CONDITIONAL_WHILE)
  {
    ir_create_goto(while_label);
  }

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
    case AST_LINE_DECLARATION:
      ir_declaration(&line->declaration);
      break;
  }
}


void ir_ast(struct ast_body *root, struct ir_stream *stream)
{
  for (int i=0; i<root->num_lines; i++)
  {
    ir_line(&root->lines[i]);
  }

  stream->items = (struct ir_item *)malloc(sizeof(struct ir_item) * ir_idx);
  stream->total = ir_idx;
  memcpy(stream->items, ir_list, sizeof(struct ir_item) * ir_idx);
}
