#include <stddef.h>

#pragma once

enum lex_token_type {
  LEX_TOKEN_TYPE,
  LEX_TOKEN_ID,
  LEX_TOKEN_SEMI_COLON,
  LEX_TOKEN_WHIESPACE,
  LEX_TOKEN_RETURN,
  LEX_TOKEN_CONSTANT,
  LEX_TOKEN_ADD, // Maybe convert to arithmetic
  LEX_TOKEN_SUB,
  LEX_TOKEN_INT, // Maybe Type
  LEX_TOKEN_EQUAL, // =
  LEX_TOKEN_IF,
  LEX_TOKEN_OPEN_SCOPE, // {
  LEX_TOKEN_CLOSE_SCOPE, // }
  LEX_TOKEN_OPEN_BRACKET, // (
  LEX_TOKEN_CLOSE_BRACKET, // )
  LEX_TOKEN_EQUIV, // ==
  LEX_TOKEN_LESS_THAN, // <
  LEX_TOKEN_GREATER_THAN, // >
  LEX_TOKEN_EOF
};

struct lex_token_t {
  enum lex_token_type type;
  char text[32];
  size_t text_len;
};

struct lex_token_list_t {
  struct lex_token_t *tokens;
  int cur_idx;
  int total_tokens;
};

int lex_tokenize_stream(
    struct lex_token_list_t *lexer_output,
    char *input,
    size_t input_size,
    size_t *token_size
);
