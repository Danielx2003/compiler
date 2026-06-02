#include "lexer.h"
#include "parse.h"
#include "scope.h"
#include "ir.h"

#include <stdio.h>
#include <stdlib.h>

struct compiler {
  struct lex_token_list_t lexer_output;
  struct ast_root root;
};

int main()
{
  struct lex_token_list_t lexer_output = {0};
  lexer_output.total_tokens = 256;
  lexer_output.cur_idx = 0;

  char text[256] = "int x = 5; if (x == 5) { int y = 5; }";

  size_t token_list_len;
  int num_lines = lex_tokenize_stream(
      &lexer_output,
      text,
      sizeof(text),
      &token_list_len
  );

 
  for (int i = 0; i < lexer_output.cur_idx ; i++)
  {
    printf("Token Type: %d --- Text: %*s\n", lexer_output.tokens[i].type, lexer_output.tokens[i].text_len, lexer_output.tokens[i].text);
  }
  printf("\n");
 
  /*
  struct ast_root *root = parse_lexer_tokens(&lexer_output, num_lines);
  if (!scope_res(root))
  {
    printf("Failed to resolve variable\n");
    return -1;
  }

  ir_ast(root);
  */
  // free(root);
  // free(lexer_output.tokens); 
}
