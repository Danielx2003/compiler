#pragma once

#include "lexer.h"
#include "scope_2.h"
#include "char_stream.h"

#include <stdio.h>

void tokenize(struct lex_token_list_t *lexer_output, char c, struct char_stream *stream);
struct lex_token_t *tokenize_char(struct char_stream *stream);
// struct lex_token_t *tokenize_span(int cur, int strt, FILE *file);
struct lex_token_t *tokenize_span(struct char_stream *stream);

struct token_ops {
  struct lex_token_t* (*tokenize_char)(struct char_stream *);
  struct lex_token_t* (*tokenize_range)(struct char_stream *);
  void (*scope_inc_num_lines)(struct scope_stack_t *);
  void (*scope_inc_num_tokens)(struct scope_stack_t *);
  void (*scope_create)(struct scope_stack_t *);
  void (*scope_pop)(struct scope_stack_t *, struct scope_t *);
};
