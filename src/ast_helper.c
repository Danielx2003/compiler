#include "ast_helper.h"

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
