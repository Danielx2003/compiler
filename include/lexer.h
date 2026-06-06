#pragma once

#include "char_stream.h"

#include <stddef.h>

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
  LEX_TOKEN_WHILE,
  LEX_TOKEN_FUNCTION,
  LEX_TOKEN_COMMA,
  LEX_TOKEN_EOF
};

struct lex_token_t {
  enum lex_token_type type;
  char text[32];
  size_t text_len;
  union {
    int num_lines;
  } ctx;
};

struct lex_token_stream {
  struct lex_token_t *data;
  int cur_idx;
  int capacity;
};

int lex_tokenize_stream(
  struct lex_token_stream *tokens,
  struct char_stream *stream
);

enum lex_token_type get_token_type_from_text(char *buf);
void add_token_to_list(struct lex_token_stream *tokens, struct lex_token_t *new_token);
struct lex_token_t* get_prev_open_scope_token(struct lex_token_stream *tokens, int tokens_since);
struct lex_token_t *create_lex_token(enum lex_token_type type, char *text, size_t text_len, void *ctx, size_t ctx_size);

