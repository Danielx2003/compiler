#include <stddef.h>

#pragma once

enum token_type {
  TOKEN_TYPE_TYPE,
  TOKEN_TYPE_ID,
  TOKEN_TYPE_SEMI_COLON,
  TOKEN_TYPE_WHIESPACE,
  TOKEN_TYPE_RETURN,
  TOKEN_TYPE_CONSTANT,
  TOKEN_TYPE_ADD, // Maybe convert to arithmetic
  TOKEN_TYPE_SUB,
  TOKEN_TYPE_INT, // Maybe Type
  TOKEN_TYPE_EQUAL,
  TOKEN_TYPE_EOF
};

struct token_t {
  enum token_type type;
  char text[32];
  size_t text_len;
};

struct token_list_t {
  struct token_t *tokens;
  int cur_idx;
  int total_tokens;
};

// Tokenizes a whole input stream, returning an array of tokens
int tokenize_stream(
    struct token_list_t *lexer_output,
    char *input,
    size_t input_size,
    size_t *token_size
);

// Tokenizes a single input, terminating at the terminator/deliminator
struct token_t tokenize_input(char *input, size_t input_size);
