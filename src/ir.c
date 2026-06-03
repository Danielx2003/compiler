#include "ir.h"

#include <string.h>
#include <stdio.h>

static int temp_count = 0;
static int label_count = 0;

struct ir_item ir_list[32] = {0};
static int ir_idx = 0;

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

void create_if(int temp, int label)
{
  struct ir_item item = {0};
  item.type = IR_ITEM_CONDITIONAL;
  item.conditional.temp = temp;
  item.conditional.label = label;
  add_to_ir_list(&item);

  // printf("ifz t%d goto L%d\n", item.conditional.temp, item.conditional.label);
}

struct ir_ret ir_expr_tail(struct ast_expr_tail *tail)
{
  struct ir_ret ret = {
    .type = IR_RET_TYPE_NULL,
    .temp = 0
  };

  if (tail == NULL || tail->type == AST_EXPR_PRIME_NULL)
  {
    return ret;
  }

  struct ir_ret tail_ret = ir_expr_tail(tail->next);

  if (tail->type == AST_EXPR_PRIME_TERM_ONLY)
  {
    if (tail->term.type == AST_TERM_ID)
    {
      ret.type = IR_RET_TYPE_TERM;
      memcpy(&ret.id, &tail->term.id, sizeof(struct ast_id));
      return ret;
    }
    else if (tail->term.type == AST_TERM_CONSTANT)
    {
      ret.type = IR_RET_TYPE_CONSTANT;
      ret.temp = tail->term.constant.value;
      return ret;
    }
  }

  ret.type = IR_RET_TYPE_TEMP;
  ret.temp = ++temp_count;
  
  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;

  if (tail_ret.type == IR_RET_TYPE_NULL)
  {
    item.assign.type = IR_ASSIGN_VALUE;
    item.assign.value.type = IR_ASSIGN_VALUE_SINGLE;
    item.assign.value.assign_op = IR_ASSIGN_OPERATOR_TYPE_NONE;

    // LHS
    item.assign.value.lhs.type = IR_TERM_TEMP;
    item.assign.value.lhs.temp = ret.temp;

    // RHS
    item.assign.value.rhs_l.type = IR_TERM_TERM;

    if (tail->term.type == AST_TERM_ID)
    {
      strcpy(item.assign.value.rhs_l.text, tail->term.id.text); 
    }
    else
    {
      ret.type = IR_RET_TYPE_CONSTANT;
      item.assign.value.rhs_l.constant = tail->term.constant.value;
    }

    // printf("t%d = %s", item.assign.value.lhs, item.assign.value.rhs.term_rhs);
  }
  else
  {
    item.assign.type = IR_ASSIGN_VALUE;
    item.assign.value.type = IR_ASSIGN_VALUE_DOUBLE;
    item.assign.value.assign_op = IR_ASSIGN_OPERATOR_TYPE_ADD;

    if (ret.type == IR_RET_TYPE_TEMP)
    {
      item.assign.value.lhs.type = IR_TERM_TEMP;
      item.assign.value.lhs.temp = ret.temp;
    }
    else if (ret.type == IR_RET_TYPE_TERM)
    {  
      item.assign.value.lhs.type = IR_TERM_TERM;
      strcpy(item.assign.value.lhs.text, ret.id.text);
    }
    else if (ret.type == IR_RET_TYPE_CONSTANT)
    {
      printf("error, tried to assign constant to lhs\n");
    }


    if (tail->term.type == AST_TERM_ID)
    {
      item.assign.value.rhs_l.type = IR_TERM_TERM;
      strcpy(item.assign.value.rhs_l.text, tail->term.id.text); 
    }
    else if (tail->term.type == AST_TERM_CONSTANT)
    {
      item.assign.value.rhs_l.type = IR_TERM_CONSTANT;
      item.assign.value.rhs_l.constant = tail->term.constant.value;
    }
    // RHS

    if (tail_ret.type == IR_RET_TYPE_TEMP)
    {
      item.assign.value.rhs_r.type = IR_TERM_TEMP;
      item.assign.value.rhs_r.temp = tail_ret.temp;
    }
    else if (tail_ret.type == IR_RET_TYPE_TERM)
    {
      item.assign.value.rhs_r.type = IR_TERM_TERM;
      strcpy(item.assign.value.rhs_r.text, tail_ret.id.text);
    }
    else if (tail_ret.type == IR_RET_TYPE_CONSTANT)
    {
      item.assign.value.rhs_r.type = IR_TERM_CONSTANT;
      item.assign.value.rhs_r.constant = tail_ret.constant;
    }
    
    /*
    printf("\n\n");
    printf("t%d = %s + ", ret.temp, expr_prime->term.id.text);
    print_ir_ret(&expr_prime_ret);
    printf("\n\n");
    */
  }

  add_to_ir_list(&item);

  printf("\n");

  return ret;
}

struct ir_ret ir_expr(struct ast_expr *expr)
{
  struct ir_ret ret;
  ret.temp = ++temp_count;
  ret.type = IR_RET_TYPE_TEMP;

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;

  
  if (expr->tail.type != AST_EXPR_PRIME_NULL)
  {
    struct ir_ret tail_ret = ir_expr_tail(&expr->tail);

    item.assign.type = IR_ASSIGN_VALUE;
    item.assign.value.type = IR_ASSIGN_VALUE_DOUBLE;
    item.assign.value.assign_op = IR_ASSIGN_OPERATOR_TYPE_ADD;

    // LHS
    item.assign.value.lhs.temp = ret.temp;

    // RHS
    if (expr->term.type == AST_TERM_ID)
    {
      item.assign.value.rhs_l.type = IR_TERM_TERM;
      strcpy(item.assign.value.rhs_l.text, expr->term.id.text); 
    }
    else
    {
      item.assign.value.rhs_l.type = IR_TERM_CONSTANT;
      item.assign.value.rhs_l.constant = expr->term.constant.value;
    }


    if (tail_ret.type == IR_RET_TYPE_TEMP)
    {
      item.assign.value.rhs_r.type = IR_TERM_TEMP;
      item.assign.value.rhs_r.temp = tail_ret.temp;
    }
    else if (tail_ret.type == IR_RET_TYPE_TERM)
    {
      item.assign.value.rhs_r.type = IR_TERM_TERM;
      strcpy(item.assign.value.rhs_r.text, tail_ret.id.text);
    }
    else if (tail_ret.type == IR_RET_TYPE_CONSTANT)
    {
      item.assign.value.rhs_r.type = IR_TERM_CONSTANT;
      item.assign.value.rhs_r.constant = tail_ret.constant;
    }

    add_to_ir_list(&item);


    /*printf("t%d = %s + ", ret.temp, expr->term.id.text);
    print_ir_ret(&expr_prime_ret);
    printf("\n");*/
  }
  else
  {
    if (expr->term.type == AST_TERM_ID)
    {
      ret.type = IR_RET_TYPE_TERM;
      memcpy(&ret.id, &expr->term.id, sizeof(struct ast_id)); 
    }
    else
    {
      ret.type = IR_RET_TYPE_CONSTANT;
      ret.constant = expr->term.constant.value;
    }
  }

  // Need to move this, defo not the right spot

  return ret;
}

struct ir_ret ir_assignment(struct ast_assignment *assign)
{
  struct ir_ret ret;
  ret.temp = ++temp_count;
  struct ir_ret expr_ret = ir_expr(&assign->expr);

  struct ir_item item = {0};
  item.type = IR_ITEM_ASSIGN;

  item.assign.type = IR_ASSIGN_VALUE;
  item.assign.value.type = IR_ASSIGN_VALUE_SINGLE;

  item.assign.value.lhs.type = IR_TERM_TERM;
  strcpy(item.assign.value.lhs.text, assign->term.id.text);

  if (expr_ret.type == IR_RET_TYPE_TEMP)
  {
    item.assign.value.rhs_l.type = IR_TERM_TEMP;
    item.assign.value.rhs_l.temp = expr_ret.temp;
  }
  else if (expr_ret.type == IR_RET_TYPE_TERM)
  {
    item.assign.value.rhs_l.type = IR_TERM_TERM;
    strcpy(item.assign.value.rhs_l.text, expr_ret.id.text);
  }
  else if (expr_ret.type == IR_RET_TYPE_CONSTANT)
  {
    item.assign.value.rhs_l.type = IR_TERM_CONSTANT;
    item.assign.value.rhs_l.constant = expr_ret.constant;
  }

  add_to_ir_list(&item);

  // printf("%s = ", assign->term.id.text);
  // print_ir_ret(&expr_ret);
  // printf("\n");
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
  struct ir_ret ret;
  struct ir_item item = {0};

  int temp_local = ++temp_count;

  /*
  printf("t%d = %s %s %s \n", 
      temp_local,
      cond->left_term.id.text,
      op_to_text(cond->op),
      cond->right_term.id.text
  );
  */

  item.type = IR_ITEM_ASSIGN;
  item.assign.type = IR_ASSIGN_CONDITION;
  item.assign.condition.lhs = temp_local;

  if (
    cond->left_term.type == AST_TERM_ID
    && cond->right_term.type == AST_TERM_ID
  )
  {
    item.assign.condition.type = IR_CONDITION_TYPE_TERM_TERM; 
    item.assign.condition.op = ast_op_to_ir(cond->op);
    strcpy(item.assign.condition.term_term.term_l, cond->left_term.id.text);
    strcpy(item.assign.condition.term_term.term_r, cond->right_term.id.text);

    printf("term term\n");
  }
  else if (
    cond->left_term.type == AST_TERM_ID
    && cond->right_term.type == AST_TERM_CONSTANT
  )
  {
    item.assign.condition.type = IR_CONDITION_TYPE_TERM_TEMP; 
    item.assign.condition.op = ast_op_to_ir(cond->op);
    strcpy(item.assign.condition.term_temp.term, cond->left_term.id.text);
    item.assign.condition.term_temp.temp = cond->right_term.constant.value;
  }
  else
  {
    printf("other\n");
  }

  add_to_ir_list(&item);

  ret.type = IR_RET_TYPE_TEMP;
  ret.temp = temp_count;
  return ret;
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
  create_if(condition_ret.temp, local_label);
  // If Block
  ir_condition_body(&cond->body);
  // goto local_label
  // Else Block
  
  // printf("L%d:\n", local_label);

  struct ir_item item = {0};
  item.type = IR_ITEM_LABEL;
  item.label.label = local_label;
  add_to_ir_list(&item);
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

void print_ir_assign_value(struct ir_assign_value *value)
{
  switch(value->type)
  {
    case IR_ASSIGN_VALUE_SINGLE:
      // LHS: temp, term, constant
      // RHS: temp, term, constant
      if (
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_TEMP
      )
      {
        printf("t%d = t%d\n", value->lhs.temp, value->rhs_l.temp);
      }
      else if (
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_TERM
      )
      {
        printf("t%d = %s\n", value->lhs.temp, value->rhs_l.text);
      }
      else if (
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_CONSTANT
      )
      {
        printf("t%d = %d\n", value->lhs.temp, value->rhs_l.constant);
      }
      // LHS = TERM
      else if (
        value->lhs.type == IR_TERM_TERM
        && value->rhs_l.type == IR_TERM_TEMP
      )
      {
        printf("%s = t%d\n", value->lhs.text, value->rhs_l.temp);
      }
      else if (
        value->lhs.type == IR_TERM_TERM
        && value->rhs_l.type == IR_TERM_TERM
      )
      {
        printf("%s = %s\n", value->lhs.text, value->rhs_l.text);
      }
      else if (
        value->lhs.type == IR_TERM_TERM
        && value->rhs_l.type == IR_TERM_CONSTANT
      )
      {
        printf("%s = %d\n", value->lhs.text, value->rhs_l.constant);
      }

      else 
      {
        printf("somehow missed a assign value single pairing\n");
      }
      break;
    case IR_ASSIGN_VALUE_DOUBLE:
      // LHS: term, temp, constant
      // RHS_L: term, temp, constant
      // RHS_R: term, temp, constant
      //
      //  temp temp temp
      //  temp temp term
      //  temp temp constant
      //  temp term temp
      //  temp term term
      //  temp term constant
      //  temp constant temp
      //  temp constant term
      //  temp constant constant
      //  term temp temp
      //  term temp term
      //  term temp constant
      //  term term temp
      //  term term term
      //  term term constant
      //  term constant temp
      //  term constant term
      //  term constant constant
      //
      if ( // temp temp temp
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_TEMP
        && value->rhs_r.type == IR_TERM_TEMP
      )
      {
        printf("t%d = t%d + t%d\n", value->lhs.temp, value->rhs_l.temp, value->rhs_r.temp);
      }
      else if ( // temp temp term
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_TEMP
        && value->rhs_r.type == IR_TERM_TERM
      )
      {
        printf("t%d = t%d + %s\n", value->lhs.temp, value->rhs_l.temp, value->rhs_r.text);
      }
      else if ( // temp temp constant
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_TEMP
        && value->rhs_r.type == IR_TERM_CONSTANT
      )
      {
        printf("t%d = t%d + %d\n", value->lhs.temp, value->rhs_l.temp, value->rhs_r.constant);
      }
      else if ( // temp term temp
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_TERM
        && value->rhs_r.type == IR_TERM_TEMP
      )
      {
        printf("t%d = %s + t%d\n", value->lhs.temp, value->rhs_l.text, value->rhs_r.temp);
      }
      else if (
        value->lhs.type == IR_TERM_TEMP
        && value->rhs_l.type == IR_TERM_CONSTANT
        && value->rhs_r.type == IR_TERM_CONSTANT
      )
      {
        printf("t%d = %d + %d\n", value->lhs.temp, value->rhs_l.constant, value->rhs_r.constant);
      }
      else 
      {
        printf("somehow missed a assign value double pairing\n");
      } 
      break;
  }
}

void print_ir_assign_condition(struct ir_condition *cond)
{
  // Update to include constants
  switch(cond->type)
  {
    case IR_CONDITION_TYPE_TEMP_TEMP:
      printf("t%d = %d %s %d\n", cond->lhs, cond->temp_temp.temp_l, ir_op_to_text(cond->op), cond->temp_temp.temp_r);
      break;
    case IR_CONDITION_TYPE_TEMP_TERM:
      printf("t%d = %d %s %s\n", cond->lhs, cond->temp_term.temp, ir_op_to_text(cond->op), cond->temp_term.term);
      break;
    case IR_CONDITION_TYPE_TERM_TEMP:
      printf("t%d = %s %s %d\n", cond->lhs, cond->term_temp.term, ir_op_to_text(cond->op), cond->term_temp.temp);
      break;
    case IR_CONDITION_TYPE_TERM_TERM:
      printf("t%d = %s %s %s\n", cond->lhs, cond->term_term.term_l, ir_op_to_text(cond->op), cond->term_term.term_r);
      break;
  }
}

void print_ir_assign(struct ir_assign *assign)
{
  switch(assign->type)
  {
    case IR_ASSIGN_VALUE:
      print_ir_assign_value(&assign->value);
      break;
    case IR_ASSIGN_CONDITION:
      print_ir_assign_condition(&assign->condition);
      break;
  }
}

void ir_ast(struct ast_body *root)
{
  // printf("%d Lines: \n", root->num_lines);
  for (int i=0; i<root->num_lines; i++)
  {
    ir_line(&root->lines[i]);
  }

  for (int i = 0; i < ir_idx; i++)
  {
    switch (ir_list[i].type)
    {
      case IR_ITEM_ASSIGN:
        print_ir_assign(&ir_list[i].assign);
        break;
      case IR_ITEM_CONDITIONAL:
        print_ir_conditional(&ir_list[i].conditional);
        break;
      case IR_ITEM_LABEL:
        print_ir_label(&ir_list[i].label);
        break;
    }
  }

}
