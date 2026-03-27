#include <stddef.h>

#pragma once

/*
 
Tokens:
- TYPE
- IDENTIFIER
- TERMINATOR


spaces treated as deliminators / transition to end state
 */

enum token_type {
  TYPE,
  IDENTIFIER,
  SEMI_COLON,
  WHIESPACE,
  RETURN,
  CONSTANT
};

struct token_t {
  enum token_type type;
  char text[32];
  size_t text_len;
};

struct token_list_t {
  struct token_t *token_list;
  int idx;
  int len;
};

// Tokenizes a whole input stream, returning an array of tokens
struct token_t* tokenize_stream(char *input, size_t input_size, size_t *token_size);

// Tokenizes a single input, terminating at the terminator/deliminator
struct token_t tokenize_input(char *input, size_t input_size);
