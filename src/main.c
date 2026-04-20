#include "lexer.h"
#include "parse.h"
#include "ir.h"

#include <stdio.h>
#include <stdlib.h>

struct compiler {
  struct token_list_t lexer_output;
  struct ast_root root;
};

int main()
{
  struct token_list_t lexer_output = {0};
  lexer_output.total_tokens = 16;
  lexer_output.cur_idx = 0;

  size_t token_list_len;
  int num_lines = tokenize_stream(
      &lexer_output,
      "int x = 5;",
      sizeof("int x = 5;"),
      &token_list_len
  );

  struct ast_root *root = parse_lexer_tokens(&lexer_output, num_lines);
  ir_ast(root);

  free(root);
  free(lexer_output.tokens);
}
