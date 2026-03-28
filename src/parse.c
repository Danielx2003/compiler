#include "parse.h"

#include <stdio.h>
#include <string.h>

static struct token_t cur_token = {0};

void parse_terminator(struct token_list_t *lexer_output)
{
  // Anchor Set:
  // TOKEN_TYPE_SEMI_COLON
  
  if (cur_token.type == TOKEN_TYPE_SEMI_COLON)
  {
    printf("%s\n", cur_token.text);
    consume_token(lexer_output);
    return;
  }

  printf("Expecting semi colon. Received %d\n", cur_token.type);
  while (
      cur_token.type != TOKEN_TYPE_ID
      && cur_token.type != TOKEN_TYPE_CONSTANT
      && cur_token.type != TOKEN_TYPE_EOF
      )
  {
    consume_token(lexer_output);
  }
}

void parse_term(
    struct token_list_t *lexer_output
)
{
  // Anchor Set:
  // TOKEN_TYPE_ADD, TOKEN_TYPE_SEMI_COLON -> we used id and constatnt before...

  if (cur_token.type == TOKEN_TYPE_ID
      || cur_token.type == TOKEN_TYPE_CONSTANT)
  {
    printf("%s\n", cur_token.text); 
    consume_token(lexer_output);
    return;
  }

  printf("Error: Expecting ID or CONSTANT . Received %d\n", cur_token.type);
  while (cur_token.type != TOKEN_TYPE_ADD
      && cur_token.type != TOKEN_TYPE_SEMI_COLON)
  {
    consume_token(lexer_output);
  }
}

void parse_expr_prime(
    struct token_list_t *lexer_output
)
{
  if (cur_token.type == TOKEN_TYPE_ADD) // change to be any arithmetic later
  {
    printf("%s\n", cur_token.text); 
    consume_token(lexer_output);
    parse_term(lexer_output);
    parse_expr_prime(lexer_output);
  }
}

void parse_expr(
    struct token_list_t *lexer_output
)
{
  parse_term(lexer_output);
  parse_expr_prime(lexer_output);
}

void parse_line(
    struct token_list_t *lexer_output
)
{
  parse_expr(lexer_output);
  parse_terminator(lexer_output);
}

void parse_lexer_tokens(struct token_list_t *lexer_output) // Equivalent to parse_program
{
  // Using the cur_idx produces side effects -> consider a different method later
  
  lexer_output->cur_idx = 0; 

  do {
    peek_token(lexer_output);

    if (cur_token.type == TOKEN_TYPE_EOF) { printf("at EOF"); continue; }
    parse_line(lexer_output);

  } while (cur_token.type != TOKEN_TYPE_EOF && lexer_output->cur_idx < lexer_output->total_tokens);
  
  printf("--- Done ---\n");
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
