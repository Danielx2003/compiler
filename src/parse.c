#include "parse.h"

#include <stdio.h>
#include <string.h>

static void parse_expression(struct token_list_t *lexer_output);

static void parse_return(struct token_list_t *lexer_output)
{
  lexer_output->cur_idx += 1;
  parse_expression(lexer_output);
}

static void parse_expression(struct token_list_t *lexer_output)
{
  struct token_t cur_token;
  peek_token(lexer_output, &cur_token);

  switch (cur_token.type)
  {
    case TOKEN_TYPE_CONSTANT:
    case TOKEN_TYPE_ID:
      return;
    default:
      printf("invalid syntax: not an expression\n");
  } 
}

void parse_lexer_tokens(struct token_list_t *lexer_output)
{
  // Using the cur_idx produces side effects -> consider a different method later
  
  lexer_output->cur_idx = 0;
  
  struct token_t token;
  do {
    peek_token(lexer_output, &token);
    consume_token(lexer_output, &token);
  } while (token.type != TOKEN_TYPE_EOF);
}

void peek_token(
    struct token_list_t *lexer_output,
    struct token_t *token
)
{
  memcpy(token, &lexer_output->tokens[lexer_output->cur_idx], sizeof(struct token_t));
}

void consume_token(
    struct token_list_t *lexer_output,
    struct token_t *token
)
{
  switch (token->type)
  {
    case TOKEN_TYPE_RETURN:
      parse_return(lexer_output);      
      break;
    default:
      break;
  }

  lexer_output->cur_idx++;
}
