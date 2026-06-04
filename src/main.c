#include "lexer.h"
#include "parse.h"
#include "scope.h"
#include "ir.h"
#include "file_stream.h"

#include <stdio.h>
#include <stdlib.h>

struct compiler {
  struct lex_token_list_t lexer_output;
  struct ast_body body;
};

int main()
{
  /*
  char *file_name = "test.yega";
  struct file_iterator *it = create_file_iterator(file_name);
  */

  

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
 
  
  struct ast_body *body = parse_lexer_tokens(&lexer_output, num_lines);
  
  if (!scope_res(body))
  {
    printf("Failed to resolve variable\n");
    return -1;
  }

  // ir_ast(body);
  // free(root);
  // free(lexer_output.tokens); 
}
