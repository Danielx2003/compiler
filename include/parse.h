#pragma once

#include "lexer.h" // Migrate tokens to own header file

/*

peek_token
consume_token

parse_xyz - should be priv - not in header

do {
  peek_token()
  consume_token()
}

while (token != end_of_file)

consume_token(token)
{
  switch(token->type)
  {
    case RETURN:
      parse_return();
      break;
    case IDENTIFIER:
      parse_id();
      break;
    default:
      printf("Not a known token\n");
      break;
  }
  tokens->idx++;
}

peek_token(tokens_list)
{
  return tokens_list[tokens->idx];
}
*/

void parse_lexer_tokens(
  struct token_list_t *lexer_output
);

void peek_token(
    struct token_list_t *lexer_output,
    struct token_t *token
);

void consume_token(struct token_list_t *lexer_output, struct token_t *token);
