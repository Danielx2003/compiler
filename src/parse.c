#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static struct token_t cur_token = {0};
int cur_line = 0;
bool error = false;

bool parse_terminator(struct token_list_t *lexer_output)
{
  // Anchor Set:
  // TOKEN_TYPE_SEMI_COLON
  
  if (cur_token.type == TOKEN_TYPE_SEMI_COLON)
  {
    consume_token(lexer_output);
    return true;
  }

  printf("Expecting semi colon. Received %*s\n", cur_token.text_len, cur_token.text);
  error = true;
  while (
      cur_token.type != TOKEN_TYPE_ID
      && cur_token.type != TOKEN_TYPE_CONSTANT
      && cur_token.type != TOKEN_TYPE_EOF)
  {
    consume_token(lexer_output);
  }

  if (cur_token.type == TOKEN_TYPE_EOF) { return false; }
  return true;
}

bool parse_term(
    struct token_list_t *lexer_output,
    struct ast_term *term
)
{
  // Anchor Set:
  // TOKEN_TYPE_ADD, TOKEN_TYPE_SEMI_COLON -> we used id and constatnt before...

  if (cur_token.type == TOKEN_TYPE_ID)
  {
    term->type = AST_TERM_TYPE_ID;
    strcpy(term->id.text, cur_token.text);
    consume_token(lexer_output);
    return true;
  }
  else if (cur_token.type == TOKEN_TYPE_CONSTANT)
  {
    term->type = AST_TERM_TYPE_CONSTANT;
    strcpy(term->id.text, cur_token.text);
    consume_token(lexer_output);
    return true;
  }

  printf("Error: Expecting ID or CONSTANT . Received %d\n", cur_token.type);
  error = true;
  while (cur_token.type != TOKEN_TYPE_ADD
      && cur_token.type != TOKEN_TYPE_SEMI_COLON
      && cur_token.type != TOKEN_TYPE_EOF)
  {
    consume_token(lexer_output);
  }

  if (cur_token.type == TOKEN_TYPE_EOF) { return false; }
  return true;
}

bool parse_expr_prime(
    struct token_list_t *lexer_output,
    struct ast_expr_prime *expr_prime,
    struct ast_expr_prime *expr_prime_parent
)
{
  if (cur_token.type == TOKEN_TYPE_ADD) // change to be any arithmetic later
  {
    expr_prime->type = AST_EXPR_PRIME_TYPE_TERM_EXPR;
    expr_prime->expr_prime = (struct ast_expr_prime*)malloc(sizeof(struct ast_expr_prime));

    consume_token(lexer_output);
    if (!parse_term(lexer_output, &expr_prime->term)
        || !parse_expr_prime(lexer_output, expr_prime->expr_prime, expr_prime))
    {
      free(expr_prime->expr_prime);
      expr_prime->expr_prime = NULL;
      return false;
    }
  }
  else
  {
    expr_prime->type = AST_EXPR_PRIME_TYPE_NULL;
    if (expr_prime_parent != NULL)
    {
      expr_prime_parent->type = AST_EXPR_PRIME_TYPE_TERM_ONLY;
    }
  }

  return true;
}

bool parse_expr(
    struct token_list_t *lexer_output,
    struct ast_expr *expr
)
{
  if (!parse_term(lexer_output, &expr->term)
      || !parse_expr_prime(lexer_output, &expr->expr_prime, NULL))
  {
    return false;
  }

  return true;
}


bool parse_equals(struct token_list_t *lexer_output)
{
  if (cur_token.type == TOKEN_TYPE_EQUAL)
  {
    consume_token(lexer_output);
    return true;
  }
  else
  {
    printf("Error: Expected =\n");
  }

  return true;
  // Otherwise consume until we reach the anchor set / FOLLOW(=)
}

bool parse_assignment(
    struct token_list_t *lexer_output,
    struct ast_assignment *assignment)
{
  // Work out anchor set / FOLLOW(assignment)

  if (cur_token.type == TOKEN_TYPE_INT)
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
    printf("Error: expected type");
  }

  parse_terminator(lexer_output);

  return true;
}

bool parse_op(
   struct token_list_t *lexer_output,
   enum ast_operator_type *op
)
{
  switch (cur_token.type)
  {
    case TOKEN_TYPE_EQUIV:
      *op = AST_OPERATOR_TYPE_EQUIV;
      break;
    case TOKEN_TYPE_LESS_THAN:
      *op = AST_OPERATOR_TYPE_LESS_THAN;
      break;
    case TOKEN_TYPE_GREATER_THAN:
      *op = AST_OPERATOR_TYPE_GREATER_THAN;
      break;
    default:
      printf("Invalid operator\n");
      return false;
  }
  consume_token(lexer_output);
  return true;
}

bool parse_condition(
  struct token_list_t *lexer_output,
  struct ast_condition *condition
)
{
  if (!parse_term(lexer_output, &condition->left_term)
      || !parse_op(lexer_output, &condition->op)
      || !parse_term(lexer_output, &condition->right_term))
  {
    return false;
  }

  printf("%s == %s", condition->left_term.id.text, condition->right_term.id.text);

  if (cur_token.type == TOKEN_TYPE_CLOSE_BRACKET)
  {
    consume_token(lexer_output);
    return true;
  }
  
  // find follow set, then while loop until we get to an element
  printf("missing close bracket\n");
}

bool parse_condition_body(
  struct token_list_t *lexer_output,
  struct ast_condition_body *body
)
{
  if (cur_token.type == TOKEN_TYPE_OPEN_SCOPE) 
  {
    consume_token(lexer_output);
    parse_assignment(lexer_output, &body->assignment);
  }
  else
  {
    printf("expecting {\n");
    // do a while loop until we reach a follow set element
  }

  if (cur_token.type == TOKEN_TYPE_CLOSE_SCOPE)
  {
    consume_token(lexer_output);
  }
  else
  { 
    printf("expecting }\n, %*s\n", cur_token.text_len, cur_token.text);
    // do a while loop until we reach a follow set element
  }
}

bool parse_if(
  struct token_list_t *lexer_output,
  struct ast_conditional *cond
)
{
  if (cur_token.type == TOKEN_TYPE_OPEN_BRACKET) 
  {
    consume_token(lexer_output);
    parse_condition(lexer_output, &cond->condition);
    parse_condition_body(lexer_output, &cond->body);
  }
  else
  {
    printf("Expecting '('\n");
    return false;
  }
}

void parse_line(
    struct token_list_t *lexer_output,
    struct ast_line *line
)
{
  if (cur_token.type == TOKEN_TYPE_IF)
  {
    line->type = AST_LINE_CONDITIONAL;
    consume_token(lexer_output);
    parse_if(lexer_output, &line->ctx.conditional);
  }
  else
  {
    line->type = AST_LINE_ASSIGNMENT;
    parse_assignment(lexer_output, &line->ctx.assignment);
  }
}

struct ast_root* parse_lexer_tokens(struct token_list_t *lexer_output, int num_lines)
{
  // Using the cur_idx produces side effects -> consider a different method later
  
  lexer_output->cur_idx = 0;
  struct ast_root *root = (struct ast_root *)calloc(1, sizeof(struct ast_root));
  root->num_lines = num_lines;

  root->lines = (struct ast_line *)malloc(sizeof(struct ast_line) * num_lines);

  do {
    peek_token(lexer_output);

    if (cur_token.type == TOKEN_TYPE_EOF) { printf("at EOF"); continue; }

    parse_line(lexer_output, &root->lines[cur_line]);
    cur_line++;
  } while (cur_token.type != TOKEN_TYPE_EOF);
  
  printf("--- Done ---\n");

  // if (!error) { print_ast_root(root); }
  // else { printf("Syntax error found\n"); }
  // free_ast(root);
  return root;
}

void peek_token(
    struct token_list_t *lexer_output
)
{
  memcpy(&cur_token, &lexer_output->tokens[lexer_output->cur_idx], sizeof(struct token_t));
}

void consume_token(
    struct token_list_t *lexer_output
)
{
  if (lexer_output->cur_idx > lexer_output->total_tokens) { return; }

  lexer_output->cur_idx++;
  peek_token(lexer_output);
}

void free_ast_expr_prime(struct ast_expr_prime *expr_prime)
{
  if (expr_prime) { return; }
  free_ast_expr_prime(expr_prime->expr_prime);
  
  if (!expr_prime->expr_prime) { return; }
  free(expr_prime->expr_prime);
}

void free_ast_expr(struct ast_expr *expr)
{
  free_ast_expr_prime(&expr->expr_prime);
}

void free_ast_assignment(struct ast_assignment *assign)
{
  free_ast_expr(&assign->expr);
}

void free_ast_line(struct ast_line *line)
{
  free_ast_assignment(&line->ctx.assignment);
}

void free_ast(struct ast_root *root)
{
  for (int i=0; i<root->num_lines; i++)
  {
    free_ast_line(&root->lines[i]);
  }

  free(root->lines);
}
