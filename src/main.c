#include "lexer.h"
#include "parse.h"
#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
  struct token_list_t lexer_output = {0};
  lexer_output.total_tokens = 16;
  lexer_output.cur_idx = 0;

  size_t token_list_len;
  int num_lines = tokenize_stream(
      &lexer_output,
      "int x = 5 + 4 + 3 + 2 + 1 + y;",
      sizeof("int x = 5 + 4 + 3 + 2 + 1 + y;"),
      &token_list_len
  );

  printf("Extracted Tokens:\n");                                                             
  for (int i = 0; i < lexer_output.cur_idx; i++)                                             
  {                                                                                          
    printf("Token: %d Text: %s\n", lexer_output.tokens[i].type, lexer_output.tokens[i].text);
  }                                                                                          
  printf("---\n");

  struct ast_root *root = parse_lexer_tokens(&lexer_output, num_lines);
  cgen_ast(root);

  free(root);
  free(lexer_output.tokens);
}
