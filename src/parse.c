#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * [ ] Update FOLLOW/SYNC sets to include EOF
 */

static struct token_t cur_token = {0};
int cur_line = 0;
bool error = false;

static void print_ast_term(struct ast_term *term, int level)
{
  for (int i=0; i<level; i++)
  {
    printf("    ");
  }

  if (term->type == AST_TERM_TYPE_ID)
  {
    printf("ID: %s\n", term->id.text);
  }
  else if (term->type == AST_TERM_TYPE_CONSTANT)
  {
    printf("Constant: %s\n", term->constant.text);
  }
}

static void print_ast_expr_prime(struct ast_expr_prime *expr_prime, int level)
{
  if (expr_prime == NULL) { return; }


  if (expr_prime->type == AST_EXPR_PRIME_TYPE_EXPR)
  {
    for (int i=0; i<level; i++)
    {
      printf("    ");
    }
    printf("Operator: +\n");
    for (int i=0; i<level; i++)
    {
      printf("    ");
    }
    printf("Left:\n");
    print_ast_term(&expr_prime->term, level+1);
    for (int i=0; i<level; i++)
    {
      printf("    ");
    }
    printf("Right:\n");
    print_ast_expr_prime(expr_prime->expr_prime, level+1);
  }
}

static void print_ast_expr(struct ast_expr *expr, int level)
{
  for (int i=0; i<level; i++)
  {
    printf("    ");
  }
  printf("Expr:\n");

  for (int i=0; i<level; i++)
  {
    printf("    ");
  }
  printf("Left: \n");
  print_ast_term(&expr->term, level+1);

  for (int i=0; i<level; i++)
  {
    printf("    ");
  }
  printf("Expr:\n");
  print_ast_expr_prime(&expr->expr_prime, level+1);
}

static void print_ast_type(int level)
{ 
  for (int i=0; i<level; i++)
  {
    printf("    ");
  }
  printf("Type: Int\n");
}

static void print_ast_assignment(struct ast_assignment *assign, int level)
{
  for (int i=0; i<level; i++)
  {
    printf("    ");
  }
  printf("Assignment:\n");

  for (int i=0; i<level; i++)
  {
    printf("    ");
  }
  printf("Left: ");

  print_ast_type(level+1);
}

static void print_ast_line(struct ast_line *line)
{
  int level = 0;
  print_ast_assignment(&line->assignment, level+1);
}

static void print_ast_root(struct ast_root *root)
{
  for (int i=0; i<root->num_lines; i++)
  {
    printf("Line %d:\n", i+1);
    print_ast_line(&root->lines[i]);
  } 

}

bool parse_terminator(struct token_list_t *lexer_output)
{
  // Anchor Set:
  // TOKEN_TYPE_SEMI_COLON
  
  if (cur_token.type == TOKEN_TYPE_SEMI_COLON)
  {
    consume_token(lexer_output);
    return true;
  }

  printf("Expecting semi colon. Received %d\n", cur_token.type);
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
    struct ast_expr_prime *expr_prime
)
{
  if (expr_prime == NULL)
  {
    return true;
  }

  expr_prime->type = AST_EXPR_PRIME_TYPE_EMPTY;
  expr_prime->expr_prime = (struct ast_expr_prime*)malloc(sizeof(struct ast_expr_prime));

  if (cur_token.type == TOKEN_TYPE_ADD) // change to be any arithmetic later
  {
    expr_prime->type = AST_EXPR_PRIME_TYPE_EXPR;

    consume_token(lexer_output);
    if (!parse_term(lexer_output, &expr_prime->term)
        || !parse_expr_prime(lexer_output, expr_prime->expr_prime))
    {
      free(expr_prime->expr_prime);
      expr_prime->expr_prime = NULL;
      return false;
    }
  }
  else
  {
    free(expr_prime->expr_prime);
    expr_prime->expr_prime = NULL;
  }

  return true;
}


bool parse_expr(
    struct token_list_t *lexer_output,
    struct ast_expr *expr
)
{
  if (!parse_term(lexer_output, &expr->term)
      || !parse_expr_prime(lexer_output, &expr->expr_prime))
  {
    return false;
  }

  return true;
}


bool parse_equals(struct token_list_t *lexer_output)
{
  if (cur_token.type == TOKEN_TYPE_EQUAL)
  {
    // Handle
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

  return true;
}

void parse_line(
    struct token_list_t *lexer_output,
    struct ast_line *line
)
{
  parse_assignment(lexer_output, &line->assignment);
  parse_terminator(lexer_output);
}

void parse_lexer_tokens(struct token_list_t *lexer_output, int num_lines)
{
  // Using the cur_idx produces side effects -> consider a different method later
  
  lexer_output->cur_idx = 0;
  struct ast_root root = {0};
  root.num_lines = num_lines;

  root.lines = (struct ast_line *)malloc(sizeof(struct ast_line) * num_lines);

  do {
    peek_token(lexer_output);

    if (cur_token.type == TOKEN_TYPE_EOF) { printf("at EOF"); continue; }

    parse_line(lexer_output, &root.lines[cur_line]);
    cur_line++;
    break;

  } while (cur_token.type != TOKEN_TYPE_EOF);
  
  printf("--- Done ---\n");

  if (!error) { print_ast_root(&root); }
  else { printf("Syntax error found\n"); }
  free_ast(&root, num_lines);
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
  if (!expr_prime->expr_prime) { return; }
  free_ast_expr_prime(expr_prime->expr_prime);
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
  free_ast_assignment(&line->assignment);
}

void free_ast(struct ast_root *root, int num_lines)
{
  for (int i=0; i<root->num_lines; i++)
  {
    free_ast_line(&root->lines[i]);
  }

  free(root->lines);
}
