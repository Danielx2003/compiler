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
    ir->constant = ast->constant;
  }
  else if (ast->type == AST_TERM_FUNC_CALL)
  {
    struct ir_item item = {
      .type = IR_ITEM_FUNC_CALL
    };

    int arg_count = 0;
    for (struct ast_arg *ptr = ast->func_call.args; ptr != NULL; ptr=ptr->next)
    {
      arg_count++;
    }

    item.func_call.args = (struct ir_term *)malloc(sizeof(struct ir_term) * arg_count);
    struct ast_arg *ptr = ast->func_call.args;
    for (int i = 0; i <arg_count; i++)
    {
      ir_set_term_from_ast(&item.func_call.args[i], &ptr->term);
      ptr = ptr->next;
    }

    item.func_call.return_temp = ++temp_count;
    strcpy(item.func_call.text, ast->func_call.id.text);
    item.func_call.num_args = arg_count;
    add_to_ir_list(&item);

    ir->type = IR_TERM_TEMP;
    ir->temp = item.func_call.return_temp;
  }
}

void ir_set_id_from_ast(struct ir_term *ir, struct ast_id *id)
{
  ir->type = IR_TERM_ID;
  strcpy(ir->text, id->text);
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
  else if (ret->type == IR_RET_TYPE_FUNC_CALL)
  {
    // rather than copying the func_call struct
    // add a ir_item_func_call to the list
    // store the return result in a temp (say t3)
    // then set this type to be IR_TERM_TEMP 
    // and set the temp to be t3 
    // this means we call the function before, store its result, and then use the result where needed (e.g. an expression)
    //

    struct ir_item item = {
      .type = IR_ITEM_FUNC_CALL
    };

    memcpy(&item.func_call, &ret->func_call, sizeof(struct ir_func_call));
    item.func_call.return_temp = ++temp_count;

    add_to_ir_list(&item);

    ir->type = IR_TERM_TEMP;
    ir->temp = item.func_call.return_temp;
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
    ret->constant = ast->constant;
  }
  else if (ast->type == AST_TERM_FUNC_CALL)
  {
    ret->type = IR_RET_TYPE_FUNC_CALL;
    strcpy(ret->func_call.text, ast->func_call.id.text);

    int arg_count = 0;
    for (struct ast_arg *ptr = ast->func_call.args; ptr != NULL; ptr=ptr->next)
    {
      arg_count++;
    }

    ret->func_call.args = (struct ir_term *)malloc(sizeof(struct ir_term) * arg_count);
    ret->func_call.num_args = arg_count;

    int i = 0;
    for (struct ast_arg *ptr = ast->func_call.args; ptr != NULL; ptr=ptr->next)
    {
      ir_set_term_from_ast(&ret->func_call.args[i], &ptr->term);
      i++;
    }
  }
}

void add_to_ir_list(struct ir_item *item)
{
  memcpy(&ir_list[ir_idx], item, sizeof(struct ir_item));
  ir_idx++;
}

void ir_create_function_name(struct ast_id *id) // change 
{
  struct ir_item item = {
    .type = IR_ITEM_FUNC_DECL
  };
  strcpy(item.function_def.name, id->text);
  add_to_ir_list(&item);
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


  if (tail->term.type == AST_TERM_FUNC_CALL)
  {
    // when we have a function call as a term, i need to do the function call, and then store the return value in a temp
    // e.g. int x = 1 + add_two(1, 2);
    // we should yield something akin to
    //
    // mov rdi, 1 ; mov arg 1 into the first slot
    // mov rsi, 2 ; arg 2 into the second
    // call add_two ; call our function
    // mov tx, rax ; mov the return value into a temporary
    //
    // TAC: - or a similar result
    // push 1
    // push 2
    // t1 = call add_two
    // t2 = 1 + t1
    // x = t2
    //
    // printf("tail term is a function call\n");
  }

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

struct ir_ret ir_var_decl(struct ast_var_decl *decl)
{
  struct ir_ret expr_ret = ir_expr(&decl->expr);

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;
  
  ir_set_id_from_ast(&item.assign.lhs, &decl->id);
  ir_set_term_from_ret(&item.assign.rhs_1, &expr_ret);
  add_to_ir_list(&item);

  return (struct ir_ret) {0};
}

struct ir_ret ir_assignment(struct ast_assignment *assign)
{
  struct ir_ret expr_ret = ir_expr(&assign->expr);

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;
  
  ir_set_id_from_ast(&item.assign.lhs, &assign->id);
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

void ir_while_loop(struct ast_while_loop *while_loop)
{
  int while_label = ++label_count;
  ir_create_label(while_label);

  struct ir_ret condition_ret = ir_condition(&while_loop->condition);
  int local_label = ++label_count;
  ir_create_if(condition_ret.temp, local_label);

  ir_condition_body(&while_loop->body);
  ir_create_goto(while_label);
  ir_create_label(local_label);

  // else - not in the language yet

}

void ir_if_stmt(struct ast_if_stmt *if_stmt)
{
  struct ir_ret condition_ret = ir_condition(&if_stmt->condition);
  int local_label = ++label_count;
  ir_create_if(condition_ret.temp, local_label);

  ir_condition_body(&if_stmt->body);
  ir_create_label(local_label);

  // else - not in the language yet
}

void ir_func_decl(struct ast_func_decl *func)
{
  ir_create_function_name(&func->id);

  int count = 0;
  for (struct ast_param *ptr = func->params; ptr != NULL; ptr=ptr->next)
  {
    count++;
  }

  /*
    this creates:
    push a,
    push b,
    after the function definition - it should be before the function call
    replace push with some way to harvest the parameters, and store them as we need them
    */

  struct ir_item item = {
    .type = IR_ITEM_FUNC_PARAMS
  };
  item.function_params.params = (struct ir_param *)malloc(sizeof(struct ir_param) * count);
  item.function_params.total = count;

  int i = 0;
  for (struct ast_param *ptr = func->params; ptr != NULL; ptr=ptr->next)
  {
    strcpy(item.function_params.params[i].text, ptr->id.text);
    i++;
  }
  add_to_ir_list(&item);

  ir_condition_body(&func->body);

  struct ir_item item_ret = {
    .type = IR_ITEM_FUNC_RET
  };
  add_to_ir_list(&item_ret);
}

void ir_line(struct ast_line *line)
{
  switch(line->type)
  {
    case AST_LINE_IF_STMT:
      ir_if_stmt(&line->if_stmt);
      break;
    case AST_LINE_ASSIGNMENT:
      ir_assignment(&line->assignment);
      break;
    case AST_LINE_VAR_DECL:
      ir_var_decl(&line->var_decl);
      break;
    case AST_LINE_FUNC_DECL:
      ir_func_decl(&line->func_decl);
      break;
    case AST_LINE_WHILE_LOOP:
      ir_while_loop(&line->while_loop);
      break;
    case AST_LINE_FUNC_CALL:
      printf("not implemented yet\n");
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
