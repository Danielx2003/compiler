#include "lexer.h"
#include "parse.h"

#include <stdio.h>

int main()
{
  struct token_list_t lexer_output = {0};
  lexer_output.total_tokens = 16;
  lexer_output.cur_idx = 0;

  size_t token_list_len;
  tokenize_stream(
      &lexer_output,
      "5+     xyz + 3 ;",
      sizeof("5+     xyz + 3 ;"),
      &token_list_len
  );

  printf("Extracted Tokens:\n");                                                             
  for (int i = 0; i < lexer_output.cur_idx; i++)                                             
  {                                                                                          
    printf("Token: %d Text: %s\n", lexer_output.tokens[i].type, lexer_output.tokens[i].text);
  }                                                                                          
  printf("---\n");

  parse_lexer_tokens(&lexer_output);
}
