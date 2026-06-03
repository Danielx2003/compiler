#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static struct lex_token_t cur_token = {0};
int cur_line = 0;
bool error = false;

bool parse_terminator(struct lex_token_list_t *lexer_output)
{
  if (cur_token.type == LEX_TOKEN_SEMI_COLON)
  {
    consume_token(lexer_output);
    return true;
  }

  printf("expecting terminator \n");
  error = true;
  while (
      cur_token.type != LEX_TOKEN_ID
      && cur_token.type != LEX_TOKEN_CONSTANT
      && cur_token.type != LEX_TOKEN_EOF)
  {
    consume_token(lexer_output);
  }

  if (cur_token.type == LEX_TOKEN_EOF) { return false; }
  return true;
}

bool parse_term(
    struct lex_token_list_t *lexer_output,
    struct ast_term *term
)
{
  if (cur_token.type == LEX_TOKEN_ID)
  {
    term->type = AST_TERM_ID;
    strcpy(term->id.text, cur_token.text);
    consume_token(lexer_output);
    return true;
  }
  else if (cur_token.type == LEX_TOKEN_CONSTANT)
  {
    term->type = AST_TERM_CONSTANT;
    term->constant.value = atoi(cur_token.text);
    consume_token(lexer_output);
    return true;
  }
  
  printf("Error Parsing Term\n");
  error = true;
  while (
    cur_token.type != LEX_TOKEN_ADD
    && cur_token.type != LEX_TOKEN_EQUIV
    && cur_token.type != LEX_TOKEN_GREATER_THAN
    && cur_token.type != LEX_TOKEN_LESS_THAN
    && cur_token.type != LEX_TOKEN_CLOSE_SCOPE
    && cur_token.type != LEX_TOKEN_CLOSE_BRACKET
    && cur_token.type != LEX_TOKEN_SEMI_COLON
    && cur_token.type != LEX_TOKEN_EOF)
  {
    consume_token(lexer_output);
  }

  if (cur_token.type == LEX_TOKEN_EOF) { return false; }
  return true;
}

bool parse_expr_tail(
    struct lex_token_list_t *lexer_output,
    struct ast_expr_tail *tail,
    struct ast_expr_tail *tail_parent
)
{
  if (cur_token.type == LEX_TOKEN_ADD) // change to be any arithmetic later
  {
    tail->type = AST_EXPR_PRIME_TERM_EXPR;
    tail->next = (struct ast_expr_tail *)malloc(sizeof(struct ast_expr_tail));

    consume_token(lexer_output);
    if (!parse_term(lexer_output, &tail->term)
        || !parse_expr_tail(lexer_output, tail->next, tail))
    {
      free(tail->next);
      tail->next = NULL;
      return false;
    }
  }
  else
  {
    tail->type = AST_EXPR_PRIME_NULL;
    if (tail_parent != NULL)
    {
      tail_parent->type = AST_EXPR_PRIME_TERM_ONLY;
    }
  }

  return true;
}

bool parse_expr(
    struct lex_token_list_t *lexer_output,
    struct ast_expr *expr
)
{
  if (!parse_term(lexer_output, &expr->term)
      || !parse_expr_tail(lexer_output, &expr->tail, NULL))
  {
    return false;
  }

  return true;
}


bool parse_equals(struct lex_token_list_t *lexer_output)
{
  if (cur_token.type == LEX_TOKEN_EQUAL)
  {
    consume_token(lexer_output);
    return true;
  }
  else
  {
    printf("Error: Expected =\n");
  }

  return true;
}

bool parse_assignment(
    struct lex_token_list_t *lexer_output,
    struct ast_assignment *assignment)
{
  if (cur_token.type == LEX_TOKEN_INT)
  {
    assignment->type = AST_TYPE_INT;
    consume_token(lexer_output);
    if (!parse_term(lexer_output, &assignment->term)
        || !parse_equals(lexer_output)
        || !parse_expr(lexer_output, &assignment->expr))
    {
      return false;
    }
  }
  else
  {
    printf("Error parsing assignment: expected int, recieved: %d, at index %d\n", cur_token.type, lexer_output->cur_idx);
    error = true;
    while (
      cur_token.type != LEX_TOKEN_SEMI_COLON
      && cur_token.type != LEX_TOKEN_CLOSE_SCOPE
      && cur_token.type != LEX_TOKEN_EOF)
    {
      consume_token(lexer_output);
    }
    if (cur_token.type == LEX_TOKEN_EOF) { return false; }
  }

  parse_terminator(lexer_output);
  
  return true;
}

bool parse_op(
   struct lex_token_list_t *lexer_output,
   enum ast_op_type *op
)
{
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
  consume_token(lexer_output);
  return true;
}

bool parse_condition(
  struct lex_token_list_t *lexer_output,
  struct ast_condition *condition
)
{
  if (!parse_term(lexer_output, &condition->left_term)
      || !parse_op(lexer_output, &condition->op)
      || !parse_term(lexer_output, &condition->right_term))
  {
    return false;
  }

  if (cur_token.type == LEX_TOKEN_CLOSE_BRACKET)
  {
    consume_token(lexer_output);
    return true;
  }

  while (
    cur_token.type != LEX_TOKEN_CLOSE_BRACKET
    && cur_token.type != LEX_TOKEN_EOF
  )
  {
    consume_token(lexer_output);
  }

  if (cur_token.type == LEX_TOKEN_EOF)
  {
    return false;
  }

}

bool parse_condition_body(
  struct lex_token_list_t *lexer_output,
  struct ast_body *body
)
{
  body->lines = (struct ast_line*)calloc(cur_token.ctx.num_lines, sizeof(struct ast_line));
  if (body->lines != NULL)
  {
    body->num_lines = cur_token.ctx.num_lines;
  }

  if (cur_token.type == LEX_TOKEN_OPEN_SCOPE)
  {
    consume_token(lexer_output);
    for (int i = 0; i < body->num_lines; i++)
    {
      parse_line(lexer_output, &body->lines[i]);
    }
    // parse_assignment(lexer_output, &body->assignment);
  }

  if (cur_token.type == LEX_TOKEN_CLOSE_SCOPE)
  {
    consume_token(lexer_output);
  }
  else
  { 
    while (
      cur_token.type != LEX_TOKEN_SEMI_COLON
      && cur_token.type != LEX_TOKEN_CLOSE_SCOPE
      && cur_token.type != LEX_TOKEN_EOF
    )
    {
      consume_token(lexer_output);
    }

    if (cur_token.type == LEX_TOKEN_EOF)
    {
      return false;
    }
  }
}

bool parse_if(
  struct lex_token_list_t *lexer_output,
  struct ast_conditional *cond
)
{
  if (cur_token.type == LEX_TOKEN_OPEN_BRACKET) 
  {
    consume_token(lexer_output);
    parse_condition(lexer_output, &cond->condition);
    parse_condition_body(lexer_output, &cond->body);
  }
  else
  {
    while (
      cur_token.type != LEX_TOKEN_SEMI_COLON
      && cur_token.type != LEX_TOKEN_CLOSE_BRACKET
      && cur_token.type != LEX_TOKEN_EOF
    )
    {
      consume_token(lexer_output);
    }

    if (cur_token.type == LEX_TOKEN_EOF)
    {
      return false;
    }
  }

  return true;
}

void parse_line(
    struct lex_token_list_t *lexer_output,
    struct ast_line *line
)
{
  if (cur_token.type == LEX_TOKEN_IF)
  {
    line->type = AST_LINE_CONDITIONAL;
    consume_token(lexer_output);
    parse_if(lexer_output, &line->conditional);
  }
  else
  {
    line->type = AST_LINE_ASSIGNMENT;
    parse_assignment(lexer_output, &line->assignment);
  }
}

struct ast_body* parse_lexer_tokens(struct lex_token_list_t *lexer_output, int num_lines)
{
  // Using the cur_idx produces side effects -> consider a different method later
  
  lexer_output->cur_idx = 0;
  struct ast_body *root = (struct ast_body *)calloc(1, sizeof(struct ast_body));
  root->num_lines = num_lines;

  root->lines = (struct ast_line *)malloc(sizeof(struct ast_line) * num_lines);

  do {
    peek_token(lexer_output);

    if (cur_token.type == LEX_TOKEN_EOF) { continue; }

    parse_line(lexer_output, &root->lines[cur_line]);
    cur_line++;
  } while (cur_token.type != LEX_TOKEN_EOF);

  if (error) { printf("Syntax Error\n"); }
  return root;
}

void peek_token(
    struct lex_token_list_t *lexer_output
)
{
  memcpy(&cur_token, &lexer_output->tokens[lexer_output->cur_idx], sizeof(struct lex_token_t));
}

void consume_token(
    struct lex_token_list_t *lexer_output
)
{
  if (lexer_output->cur_idx > lexer_output->total_tokens) { return; }

  lexer_output->cur_idx++;
  peek_token(lexer_output);
}

void free_ast_expr_tail(struct ast_expr_tail *tail)
{
  if (tail) { return; }
  free_ast_expr_tail(tail->next);
  
  if (!tail->next) { return; }
  free(tail->next);
}

void free_ast_expr(struct ast_expr *expr)
{
  free_ast_expr_tail(&expr->tail);
}

void free_ast_assignment(struct ast_assignment *assign)
{
  free_ast_expr(&assign->expr);
}

void free_ast_line(struct ast_line *line)
{
  free_ast_assignment(&line->assignment);
}

void free_ast(struct ast_body *root)
{
  for (int i=0; i<root->num_lines; i++)
  {
    free_ast_line(&root->lines[i]);
  }

  free(root->lines);
}
