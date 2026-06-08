#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static struct lex_token_t cur_token = {0};
int cur_line = 0;
bool error = false;

bool parse_terminator(struct lex_token_stream *tokens)
{
  if (parser_match(tokens, LEX_TOKEN_SEMI_COLON))
  {
    parser_consume(tokens);
    return true;
  }

  printf("expecting terminator \n");
  error = true;
  while (
      !parser_match(tokens, LEX_TOKEN_ID)
      && !parser_match(tokens, LEX_TOKEN_CONSTANT)
      && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  return true;
}

bool parse_id(struct lex_token_stream *tokens, struct ast_id *id)
{
  if (parser_match(tokens, LEX_TOKEN_ID))
  {
    strcpy(id->text, parser_peek(tokens).text);
    parser_consume(tokens);
    return true;
  }

  while (
    !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return false;
  }

  return true;
}

bool parse_term(
    struct lex_token_stream *tokens,
    struct ast_term *term
)
{
  if (parser_match(tokens, LEX_TOKEN_ID))
  {
    if (parser_peek_n(tokens, 1).type == LEX_TOKEN_OPEN_BRACKET)
    {
      term->type = AST_TERM_FUNC_CALL;
      strcpy(term->func_call.id.text, parser_peek(tokens).text);
      
      // Consume ID
      parser_consume(tokens);
      // Consume Open Bracket
      parser_consume(tokens);

      parse_args(tokens, &term->func_call.args);
      
      if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
      {
        goto cleanup;
      }
      // Consume Closed Bracket
      parser_consume(tokens);
    }
    else
    {
      strcpy(term->id.text, parser_peek(tokens).text);
      term->type = AST_TERM_ID;
      
      // Consume ID
      parser_consume(tokens);
    }

    return true;
  }
  else if (parser_match(tokens, LEX_TOKEN_CONSTANT))
  {
    term->type = AST_TERM_CONSTANT;
    term->constant = atoi(parser_peek(tokens).text);
    parser_consume(tokens);
    return true;
  }
  else
  {
    parser_consume(tokens);
    goto cleanup;
  }

  return true;
 
cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_ADD)
    && !parser_match(tokens, LEX_TOKEN_SUB)
    && !parser_match(tokens, LEX_TOKEN_EQUIV)
    && !parser_match(tokens, LEX_TOKEN_GREATER_THAN)
    && !parser_match(tokens, LEX_TOKEN_LESS_THAN)
    && parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_EOF))
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  return true;
}

bool parse_expr_tail(
    struct lex_token_stream *tokens,
    struct ast_expr_tail **tail
)
{
  if (parser_match(tokens, LEX_TOKEN_ADD))
  { 
    struct ast_expr_tail *new_tail = (struct ast_expr_tail *)malloc(sizeof(struct ast_expr_tail));
    // tail->next = (struct ast_expr_tail *)malloc(sizeof(struct ast_expr_tail));

    parser_consume(tokens);
    if (!parse_term(tokens, &new_tail->term)
        || !parse_expr_tail(tokens, &new_tail->next))
    {
      free(new_tail);
      return false;
    }

    *tail = new_tail;
  }
  else
  {
    *tail = NULL;
  }

  return true;
}

bool parse_expr(
    struct lex_token_stream *tokens,
    struct ast_expr *expr
)
{
  expr->tail = NULL;

  if (!parse_term(tokens, &expr->term)
      || !parse_expr_tail(tokens, &expr->tail))
  {
    return false;
  }

  return true;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_COMMA)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_EOF))
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  return true;

}


bool parse_equals(struct lex_token_stream *tokens)
{
  if (parser_match(tokens, LEX_TOKEN_EQUAL))
  {
    parser_consume(tokens);
    return true;
  }
  else
  {
    printf("Error: Expected =\n");
  }

  return true;
}

bool parse_func_decl(struct lex_token_stream *tokens, struct ast_func_decl *decl)
{
  if (!parser_match(tokens, LEX_TOKEN_INT))
  {
    parser_consume(tokens);
    goto cleanup;
  }

  parser_consume(tokens);

  if (!parser_match(tokens, LEX_TOKEN_ID))
  {
    goto cleanup;
  }


  parse_id(tokens, &decl->id);  

  if (!parser_match(tokens, LEX_TOKEN_OPEN_BRACKET))
  {
    goto cleanup;  
  }

  parser_consume(tokens);

  if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  { 
    // If not a closed bracket, means we have function params
    parse_params(tokens, &decl->params);
  }


  if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  { 
    goto cleanup;
  }

  parser_consume(tokens);
  if (!parse_body(tokens, &decl->body))
  {
    return false;
  }
  
  return true;

cleanup:
  error = true;
  while (
    // !parser_match(tokens, LEX_TOKEN_INT)
    // !parser_match(tokens, LEX_TOKEN_IF)
    // && !parser_match(tokens, LEX_TOKEN_WHILE)
    // && !parser_match(tokens, LEX_TOKEN_ID)
    !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    // && !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }
  if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }

  return true;
}

bool parse_var_decl(
    struct lex_token_stream *tokens,
    struct ast_var_decl *declaration)
{
  if (parser_match(tokens, LEX_TOKEN_INT)) // will change to LEX_TOKEN_TYPE in the future
  {
    declaration->type = AST_TYPE_INT;
    parser_consume(tokens);
    if (!parse_id(tokens, &declaration->id)
        || !parse_equals(tokens)
        || !parse_expr(tokens, &declaration->expr))
    {
      return false;
    }
  }
  else
  {
    printf("Error parsing var decl: expected int, recieved: %d, at index %d\n", parser_peek(tokens).type, tokens->cur_idx);
    error = true;
    while (
      !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
      && !parser_match(tokens, LEX_TOKEN_EOF) // says to remove EOF Token from FOLLOW_LIST (unsure)
    )
    {
      parser_consume(tokens);
    }

    if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  }

  return true;
}

bool parse_assignment(
    struct lex_token_stream *tokens,
    struct ast_assignment *assignment)
{
  if (parser_match(tokens, LEX_TOKEN_ID))
  {
    if (!parse_id(tokens, &assignment->id)
        || !parse_equals(tokens)
        || !parse_expr(tokens, &assignment->expr))
    {
      return false;
    }

  }
  else
  {
    printf("Error parsing assignment: expected id, recieved: %d, at index %d\n", parser_peek(tokens).type, tokens->cur_idx);
    error = true;
    while (
      !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
      && !parser_match(tokens, LEX_TOKEN_EOF) // says EOF not in FOLLOW_SET
    )
    {
      parser_consume(tokens);
    }

    if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  }

  
  return true;
}

bool parse_op(
   struct lex_token_stream *tokens,
   enum ast_op_type *op
)
{
  cur_token = parser_peek(tokens);
  switch (cur_token.type)
  {
    case LEX_TOKEN_EQUIV:
      *op = AST_OP_EQUIV;
      break;
    case LEX_TOKEN_LESS_THAN:
      *op = AST_OP_LESS_THAN;
      break;
    case LEX_TOKEN_GREATER_THAN:
      *op = AST_OP_GREATER_THAN;
      break;
    default:
      printf("Invalid operator\n");
      return false;
  }
  parser_consume(tokens);
  return true;
}

bool parse_condition(
  struct lex_token_stream *tokens,
  struct ast_condition *condition
)
{
  if (!parse_term(tokens, &condition->left_term)
      || !parse_op(tokens, &condition->op)
      || !parse_term(tokens, &condition->right_term))
  {
    goto cleanup;
  }

  if (parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    parser_consume(tokens);
    return true;
  }

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return false;
  }

  if (parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    parser_consume(tokens);
  }

  return true;
}

bool parse_body(
  struct lex_token_stream *tokens,
  struct ast_body *body
)
{
  body->lines = (struct ast_line*)calloc(parser_peek(tokens).ctx.num_lines, sizeof(struct ast_line));
  if (body->lines != NULL)
  {
    body->num_lines = parser_peek(tokens).ctx.num_lines;
  }

  if (parser_match(tokens, LEX_TOKEN_OPEN_SCOPE))
  {
    parser_consume(tokens);
    for (int i = 0; i < body->num_lines; i++)
    {
      parse_line(tokens, &body->lines[i]);
    }
  }

  if (parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE))
  {
    parser_consume(tokens);
    return true;
  }
  else
  { 
    while (
      !parser_match(tokens, LEX_TOKEN_INT)
      && !parser_match(tokens, LEX_TOKEN_IF)
      && !parser_match(tokens, LEX_TOKEN_WHILE)
      && !parser_match(tokens, LEX_TOKEN_ID)
      && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
      && !parser_match(tokens, LEX_TOKEN_EOF)
    )
    {
      parser_consume(tokens);
    }

    if (parser_match(tokens, LEX_TOKEN_EOF))
    {
      return false;
    }
  }
}

bool parse_while(
  struct lex_token_stream *tokens,
  struct ast_while_loop *while_loop
)
{
  if (parser_match(tokens, LEX_TOKEN_OPEN_BRACKET))
  {
    parser_consume(tokens);
    parse_condition(tokens, &while_loop->condition);
    parse_body(tokens, &while_loop->body);
  }
  else
  {
    error = true;
    while (
      !parser_match(tokens, LEX_TOKEN_INT)
      && !parser_match(tokens, LEX_TOKEN_IF)
      && !parser_match(tokens, LEX_TOKEN_WHILE)
      && !parser_match(tokens, LEX_TOKEN_ID)
      && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
      && !parser_match(tokens, LEX_TOKEN_EOF)
    )
    {
      parser_consume(tokens);
    }

    if (parser_match(tokens, LEX_TOKEN_EOF))
    {
      return false;
    }
  }

  return true;
}

bool parse_if(
  struct lex_token_stream *tokens,
  struct ast_if_stmt *if_stmt
)
{
  if (parser_match(tokens, LEX_TOKEN_OPEN_BRACKET))
  {
    parser_consume(tokens);
    parse_condition(tokens, &if_stmt->condition);
    parse_body(tokens, &if_stmt->body);
  }
  else
  {
    // {"int", "if", "while", id, EOF, "}"}
    error = true;
    while (
      !parser_match(tokens, LEX_TOKEN_INT)
      && !parser_match(tokens, LEX_TOKEN_IF)
      && !parser_match(tokens, LEX_TOKEN_WHILE)
      && !parser_match(tokens, LEX_TOKEN_ID)
      && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
      && !parser_match(tokens, LEX_TOKEN_EOF)
    )
    {
      parser_consume(tokens);
    }

    if (parser_match(tokens, LEX_TOKEN_EOF))
    {
      // return false on EOF
      return false;
    }
  }

  // return true when parse_if failed, but still more tokens to parse
  return true;
}

bool parse_params_next(struct lex_token_stream *tokens, struct ast_param **param)
{
  if (!parser_match(tokens, LEX_TOKEN_COMMA))
  {
    // *param = NULL;
    return true;
  }

  struct ast_param *new_param = (struct ast_param *)malloc(sizeof(struct ast_param) * 1);

  // Consume LEX_TOKEN_COMMA
  parser_consume(tokens);

  // If not a type, error
  if (!parser_match(tokens, LEX_TOKEN_INT))
  {
    printf("Parse_params_next: Expected Type Int, recieved: %d\n", parser_peek(tokens).type);
    free(new_param);
    *param = NULL;
    goto cleanup;
  }

  parser_consume(tokens);

  if (!parser_match(tokens, LEX_TOKEN_ID))
  {
    printf("Parse_params_next: Expected Type ID, recieved: %d\n", parser_peek(tokens).type);
    free(new_param);
    *param = NULL;
    goto cleanup;
  }

  strcpy(new_param->id.text, parser_peek(tokens).text);
  new_param->type = AST_TYPE_INT;
  parser_consume(tokens);

  if (!parse_params_next(tokens, &new_param->next))
  {
    free(new_param);
    *param = NULL;
    return false;
  }

  *param = new_param;

  return true;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return false;
  }
  return false;
}

bool parse_params(struct lex_token_stream *tokens, struct ast_param **param)
{
  if (!parser_match(tokens, LEX_TOKEN_INT))
  {
    printf("expected int: got %d\n", parser_peek(tokens).type);
    goto cleanup;
  }

  parser_consume(tokens);

  if (!parser_match(tokens, LEX_TOKEN_ID))
  {
    printf("expected id: got %d\n", parser_peek(tokens).type);
    goto cleanup;
  }

  *param = (struct ast_param *)malloc(sizeof(struct ast_param) * 1);
  strcpy((*param)->id.text, parser_peek(tokens).text);
  (*param)->type = AST_TYPE_INT;
  parser_consume(tokens);

  (*param )->next = NULL;
  if(!parse_params_next(tokens, &(*param)->next))
  {
    return false;
  }

  return true;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return false;
  }
  return false;
}

bool parse_args_next(struct lex_token_stream *tokens, struct ast_arg **arg)
{
  if (!parser_match(tokens, LEX_TOKEN_COMMA))
  {
    // *param = NULL;
    return true;
  }

  struct ast_arg *new_arg = (struct ast_arg *)malloc(sizeof(struct ast_arg) * 1);

  // Consume LEX_TOKEN_COMMA
  parser_consume(tokens);

  if (
    !parser_match(tokens, LEX_TOKEN_ID)
    && !parser_match(tokens, LEX_TOKEN_CONSTANT)
  )
  {
    free(new_arg);
    *arg = NULL;
    goto cleanup;
  }

  if (parser_peek(tokens).type == LEX_TOKEN_ID)
  {
    strcpy(new_arg->term.id.text, parser_peek(tokens).text);
    new_arg->term.type = AST_TERM_ID;
  }
  else if (parser_peek(tokens).type == LEX_TOKEN_CONSTANT)
  {
    new_arg->term.constant = atoi(parser_peek(tokens).text);
    new_arg->term.type = AST_TERM_CONSTANT;
  }
  parser_consume(tokens);

  if (!parse_args_next(tokens, &new_arg->next))
  {
    free(new_arg);
    *arg = NULL;
    return false;
  }

  *arg = new_arg;

  return true;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return false;
  }
  return false;
}

bool parse_args(struct lex_token_stream *tokens, struct ast_arg **arg)
{
  if (
    !parser_match(tokens, LEX_TOKEN_ID) 
    && !parser_match(tokens, LEX_TOKEN_CONSTANT)
    )
  {

    goto cleanup;
  }

  *arg = (struct ast_arg *)malloc(sizeof(struct ast_arg) * 1);

  // use parse_term?
  if (parser_peek(tokens).type == LEX_TOKEN_ID)
  {
    strcpy((*arg)->term.id.text, parser_peek(tokens).text);
  }
  else if (parser_peek(tokens).type == LEX_TOKEN_CONSTANT)
  {
    (*arg)->term.constant = atoi(parser_peek(tokens).text);
  }
  
  parser_consume(tokens);

  (*arg)->next = NULL;
  if(!parse_args_next(tokens, &(*arg)->next))
  {
    return false;
  }

  return true;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return false;
  }
  return false;
}


void parse_function_def(struct lex_token_stream *tokens, struct ast_func_decl *func)
{
  if (!parser_match(tokens, LEX_TOKEN_INT))
  {
    goto cleanup;
  }

  func->return_type = AST_TYPE_INT; // Change to actually use the type when new ones are added
  parser_consume(tokens);

  // Update to be a parse_identifier, as parse_term allows for constants
  parse_id(tokens, &func->id);

  if (!parser_match(tokens, LEX_TOKEN_OPEN_BRACKET))
  {    
    goto cleanup;
  }

  parser_consume(tokens);
  func->params = NULL;

  if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    parse_params(tokens, &func->params);
  }

  if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    goto cleanup;
  }

  parser_consume(tokens);

  if (!parse_body(tokens, &func->body))
  {
  }

  return;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }
  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return;
  }
  return;
}

void parse_func_call(struct lex_token_stream *tokens, struct ast_func_call *call)
{
  if (!parser_match(tokens, LEX_TOKEN_ID))
  {
    goto cleanup;
  }

  parse_id(tokens, &call->id);

  if (!parser_match(tokens, LEX_TOKEN_OPEN_BRACKET))
  {
    goto cleanup;
  }

  // Adjust this logic(?)
  // The idea is: if we have a close bracket, then there are no arguments to this function
  // Otherwise, parse the args, then check we finish on a close bracket

  parser_consume(tokens);

  if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    parse_args(tokens, &call->args);
  }

  if (!parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    goto cleanup;
  }

  // parser_consume(tokens);
  parse_terminator(tokens);
  return;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }
  if (parser_match(tokens, LEX_TOKEN_EOF))
  {
    return;
  }
  return;
}

void parse_line(
    struct lex_token_stream *tokens,
    struct ast_line *line
)
{
  // struct parse_response resp = {0};

  if (parser_match(tokens, LEX_TOKEN_IF))
  {
    parser_consume(tokens);
    line->type = AST_LINE_IF_STMT;
    parse_if(tokens, &line->if_stmt);
  }
  else if (parser_match(tokens, LEX_TOKEN_WHILE))
  {
    parser_consume(tokens);
    line->type = AST_LINE_WHILE_LOOP;
    parse_while(tokens, &line->while_loop);
  }
  else if (parser_match(tokens, LEX_TOKEN_INT))
  {
    // Either Variable Declaration or Function Declaration
    if (parser_peek_n(tokens, 2).type ==  LEX_TOKEN_EQUAL)
    {
      // variable declaration
      line->type = AST_LINE_VAR_DECL;
      parse_var_decl(tokens, &line->var_decl);
      parse_terminator(tokens);
    }
    else if (parser_peek_n(tokens, 2).type == LEX_TOKEN_OPEN_BRACKET)
    {
      line->type = AST_LINE_FUNC_DECL;
      parse_func_decl(tokens, &line->func_decl);
    }
    else
    {
      // parser_consume(tokens);
      goto cleanup;
    }
  }
  else if (parser_match(tokens, LEX_TOKEN_ID))
  {
    if (parser_peek_n(tokens, 1).type == LEX_TOKEN_EQUAL)
    {
      line->type = AST_LINE_ASSIGNMENT;
      parse_assignment(tokens, &line->assignment);
      parse_terminator(tokens);
    }
    else if (parser_peek_n(tokens, 1).type ==  LEX_TOKEN_OPEN_BRACKET)
    {
      printf("line is function call\n");
      line->type = AST_LINE_FUNC_CALL;
      parse_func_call(tokens, &line->func_call);
      parse_terminator(tokens);
    }
    else
    {
      parser_consume(tokens);
      goto cleanup;
    }
  }
  else 
  {
    parser_consume(tokens);
    goto cleanup;
  }

  return;

cleanup:
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_INT)
    && !parser_match(tokens, LEX_TOKEN_IF)
    && !parser_match(tokens, LEX_TOKEN_WHILE)
    && !parser_match(tokens, LEX_TOKEN_ID)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
    && !parser_match(tokens, LEX_TOKEN_EOF)
  )
  {
    parser_consume(tokens);
  }

  return;
}

struct ast_body* parse_lexer_tokens(struct lex_token_stream *tokens, int num_lines)
{
  // Using the cur_idx produces side effects -> consider a different method later
  
  tokens->cur_idx = 0;
  struct ast_body *root = (struct ast_body *)calloc(1, sizeof(struct ast_body));
  root->num_lines = num_lines;


  root->lines = (struct ast_line *)malloc(sizeof(struct ast_line) * num_lines);

  do {
    if (parser_match(tokens, LEX_TOKEN_EOF)) { continue; }

    parse_line(tokens, &root->lines[cur_line]);
    cur_line++;
  } while (!parser_match(tokens, LEX_TOKEN_EOF));

  if (error) { printf("Syntax Error\n"); }
  return root;
}

struct lex_token_t parser_peek(struct lex_token_stream *tokens)
{
  return tokens->data[tokens->cur_idx];
}

struct lex_token_t parser_peek_n(struct lex_token_stream *tokens, int n)
{
  return tokens->data[tokens->cur_idx+n];
}

void parser_consume(struct lex_token_stream *tokens)
{
  if (tokens->cur_idx >= tokens->capacity) { return; }

  tokens->cur_idx++;
}

bool parser_match(struct lex_token_stream *tokens, enum lex_token_type expected_type)
{
  if (parser_peek(tokens).type == expected_type)
  {
    return true;
  }
  return false;
}
