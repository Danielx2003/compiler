#include "ir_helper.h"

#include <stdio.h>

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


void print_ir_term(struct ir_term *term)
{
  switch(term->type)
  {
    case IR_TERM_TEMP:
      printf("t%d", term->temp);
      break;
    case IR_TERM_ID:
      printf("%s", term->text);
      break;
    case IR_TERM_CONSTANT:
      printf("%d", term->constant);
      break;
    case IR_TERM_FUNC_CALL:
      printf("func call\n");
      break;
  }
}

void print_ir_op(enum ir_op op)
{
  switch (op)
  {
    case IR_ASSIGN_OP_ADD:
      printf("+");
      break;
    case IR_ASSIGN_OP_SUBTRACT:
      printf("-");
      break;
    case IR_ASSIGN_OP_EQUIV:
      printf("==");
      break;
    case IR_ASSIGN_OP_LESS_THAN:
      printf("<");
      break;
    case IR_ASSIGN_OP_GREATER_THAN:
      printf(">");
      break;
  }
}

void print_ir_assign(struct ir_assign *assign)
{
  print_ir_term(&assign->lhs);
  printf("=");
  print_ir_term(&assign->rhs_1);
  print_ir_op(assign->op);
  print_ir_term(&assign->rhs_2);
  printf("\n");
}

void print_ir_label(struct ir_label *label)
{
  printf("L%d:\n", label->label);
}

void print_ir_goto(struct ir_goto *go_to)
{
  printf("goto L%d\n", go_to->label);
}

void print_ir_conditional(struct ir_conditional *cond)
{
  printf("ifz t%d goto L%d\n", cond->temp, cond->label);
}

void print_ir_function_def(struct ir_function_def *func)
{
  printf("%s (func_def):\n", func->name);
}

void print_ir_function_params(struct ir_params *params)
{
  for (int i = 0; i < params->total; i++)
  {
    printf("arg %s\n", params->params[i].text);
  }
}

void print_ir_func_call(struct ir_func_call *func)
{
  for (int i = 0; i < func->num_args; i++)
  {
    printf("push ");
    print_ir_term(&func->args[i]);
    printf("\n");
  }

  printf("t%d=call %s\n", func->return_temp, func->text);
}

void print_ir_ret(struct ir_term *term)
{
  printf("ret ");
  print_ir_term(term);
  printf("\n");
}

void print_ir_item(struct ir_item *item)
{
  switch(item->type)
  {
    case IR_ITEM_ASSIGN:
      print_ir_assign(&item->assign);
      break;
    case IR_ITEM_LABEL:
      print_ir_label(&item->label);
      break;
    case IR_ITEM_CONDITIONAL:
      print_ir_conditional(&item->conditional);
      break;
    case IR_ITEM_GOTO:
      print_ir_goto(&item->go_to);
      break;
    case IR_ITEM_FUNC_DECL:
      print_ir_function_def(&item->function_def);
      break;
    case IR_ITEM_FUNC_PARAMS:
      print_ir_function_params(&item->function_params);
      break;
    case IR_ITEM_FUNC_RET:
      print_ir_ret(&item->ret);
      break;
    case IR_ITEM_FUNC_CALL:
      print_ir_func_call(&item->func_call);
      break;
    default:
      printf("missed a type: %d\n", item->type);
  }
}
