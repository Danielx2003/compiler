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

bool parse_term(
    struct lex_token_stream *tokens,
    struct ast_term *term
)
{
  if (parser_match(tokens, LEX_TOKEN_ID))
  {
    term->type = AST_TERM_ID;
    strcpy(term->id.text, parser_peek(tokens).text);
    parser_consume(tokens);
    return true;
  }
  else if (parser_match(tokens, LEX_TOKEN_CONSTANT))
  {
    term->type = AST_TERM_CONSTANT;
    term->constant.value = atoi(parser_peek(tokens).text);
    parser_consume(tokens);
    return true;
  }
  
  printf("Error Parsing Term\n");
  error = true;
  while (
    !parser_match(tokens, LEX_TOKEN_ADD)
    && !parser_match(tokens, LEX_TOKEN_EQUIV)
    && !parser_match(tokens, LEX_TOKEN_GREATER_THAN)
    && !parser_match(tokens, LEX_TOKEN_LESS_THAN)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
    && !parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET)
    && !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
    && !parser_match(tokens, LEX_TOKEN_EOF))
  {
    parser_consume(tokens);
  }

  if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  return true;
}
/*
bool parse_expr_tail(
    struct lex_token_stream *tokens,
    struct ast_expr_tail *tail
)
{
  if (cur_token.type == LEX_TOKEN_ADD) // change to be any arithmetic later
  {
    tail->next = (struct ast_expr_tail *)malloc(sizeof(struct ast_expr_tail));

    parser_consume(tokens);
    if (!parse_term(tokens, &tail->term)
        || !parse_expr_tail(tokens, tail->next))
    {
      free(tail->next);
      tail->next = NULL;
      return false;
    }
  }

  return true;
}
*/
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
  // expr->tail = (struct ast_expr_tail *)malloc(sizeof(struct ast_expr_tail));

  if (!parse_term(tokens, &expr->term)
      || !parse_expr_tail(tokens, &expr->tail))
  {
    return false;
  }

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

bool parse_assignment(
    struct lex_token_stream *tokens,
    struct ast_assignment *assignment)
{
  if (parser_match(tokens, LEX_TOKEN_INT))
  {
    assignment->type = AST_TYPE_INT;
    parser_consume(tokens);
    if (!parse_term(tokens, &assignment->term)
        || !parse_equals(tokens)
        || !parse_expr(tokens, &assignment->expr))
    {
      return false;
    }

  }
  else
  {
    printf("Error parsing assignment: expected int, recieved: %d, at index %d\n", parser_peek(tokens).type, tokens->cur_idx);
    error = true;
    while (
      !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
      && !parser_match(tokens, LEX_TOKEN_CLOSE_SCOPE)
      && !parser_match(tokens, LEX_TOKEN_EOF))
    {
      parser_consume(tokens);
    }
    if (parser_match(tokens, LEX_TOKEN_EOF)) { return false; }
  }

  parse_terminator(tokens);
  
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
    return false;
  }

  if (parser_match(tokens, LEX_TOKEN_CLOSE_BRACKET))
  {
    parser_consume(tokens);
    return true;
  }

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
}

bool parse_condition_body(
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
  }
  else
  { 
    while (
      !parser_match(tokens, LEX_TOKEN_SEMI_COLON)
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

bool parse_if(
  struct lex_token_stream *tokens,
  struct ast_conditional *cond
)
{
  if (parser_match(tokens, LEX_TOKEN_OPEN_BRACKET))
  {
    parser_consume(tokens);
    parse_condition(tokens, &cond->condition);
    parse_condition_body(tokens, &cond->body);
  }
  else
  {
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
  }

  return true;
}

void parse_line(
    struct lex_token_stream *tokens,
    struct ast_line *line
)
{
  if (parser_match(tokens, LEX_TOKEN_IF))
  {
    parser_consume(tokens);
    line->type = AST_LINE_CONDITIONAL;
    parse_if(tokens, &line->conditional);
  }
  else
  {
    line->type = AST_LINE_ASSIGNMENT;
    parse_assignment(tokens, &line->assignment);
  }
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
  // if (tokens->cur_idx >= tokens->capacity) { return NULL; }
  return tokens->data[tokens->cur_idx];
  // memcpy(&cur_token, &tokens->data[tokens->cur_idx], sizeof(struct lex_token_t));
}

void parser_consume(struct lex_token_stream *tokens)
{
  if (tokens->cur_idx >= tokens->capacity) { return; }

  tokens->cur_idx++;
  // parser_peek(tokens);
}

bool parser_match(struct lex_token_stream *tokens, enum lex_token_type expected_type)
{
  if (parser_peek(tokens).type == expected_type)
  {
    return true;
  }
  return false;
}
