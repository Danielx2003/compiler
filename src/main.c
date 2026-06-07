#include "codegen.h"
#include "file_stream.h"
#include "ir.h"
#include "ir_helper.h"
#include "lexer.h"
#include "parse.h"
#include "scope.h"

#include <stdio.h>
#include <stdlib.h>

struct compiler {
  struct lex_token_stream lexer_output;
  struct ast_body body;
};

int main()
{
  FILE *file;
  file = fopen("test.yega", "r");
  if (file == NULL)
  {
    printf("Failed to open file\n");
    return 0;
  }

  struct file_stream_state file_state = {0};
  struct char_stream file_stream = char_stream_from_file(&file_state, file);

  struct lex_token_stream tokens = {
    .capacity = 256,
    .cur_idx = 0,
  };

  int num_lines = lex_tokenize_stream(
    &tokens,
    &file_stream
  );
 
  for (int i = 0; i < tokens.cur_idx ; i++)
  {
    printf("Token Type: %d --- Text: %s\n", tokens.data[i].type, tokens.data[i].text);
  }
  printf("\n");
 
  
  struct ast_body *body = parse_lexer_tokens(&tokens, num_lines); 
  printf("Done Parsing\n");
  /*
  if (!scope_res(body))
  {
    printf("Failed to resolve variable\n");
    return -1;
  }

  
  struct ir_stream ir_stream = {0};
  ir_ast(body, &ir_stream);

  for (int i = 0; i < ir_stream.total; i++)
  {
    print_ir_item(&ir_stream.items[i]);
  }
  printf("\n");
  */
  // generate_yasm(&ir_stream);
  // free(root);
  // free(lexer_output.tokens); 
}
