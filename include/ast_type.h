#pragma once

enum ast_conditional_type {
  AST_CONDITIONAL_NULL,
  AST_CONDITIONAL_IF,
  AST_CONDITIONAL_WHILE
};

enum ast_line_type {
  AST_LINE_NULL,
  AST_LINE_ASSIGNMENT,
  AST_LINE_CONDITIONAL,
  AST_LINE_DECLARATION,
  AST_LINE_FUNCTION_DEF,
  AST_LINE_FUNCTION_CALL
};

enum ast_type {
  // Will introduce family, and other things later -> make it a struct
  AST_TYPE_INT 
};

enum ast_term_type {
  AST_TERM_ID,
  AST_TERM_CONSTANT
};

enum ast_op_type {
  AST_OP_ADD,
  AST_OP_SUBTRACT,
  AST_OP_EQUIV,
  AST_OP_LESS_THAN,
  AST_OP_GREATER_THAN  
};
