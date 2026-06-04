#include "tokenize.h"
#include "lexer.h"

const struct token_ops TOKEN_REGISTERY[256] = {
  [';'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_lines = scope_inc_num_lines, .scope_inc_num_tokens = scope_inc_num_tokens },
  [' '] = { .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['\n'] = { .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['+'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['-'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['='] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['{'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens, .scope_create = create_scope },
  ['}'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens, .scope_pop = pop_scope },
  ['('] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  [')'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['<'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
  ['>'] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_span, .scope_inc_num_tokens = scope_inc_num_tokens },
};

struct lex_token_t *tokenize_span(struct char_stream *stream)
{
  char buf[32];

  int read = stream->api->read_prev(stream->self, buf, stream->api->get_window_size(stream->self));
  if (read == 0)
  {
    return NULL;
  }
    
  buf[read] = '\0';

  struct lex_token_t *token_ptr = create_lex_token(
    get_token_type_from_text(buf),
    buf,
    stream->api->get_window_size(stream->self),
    NULL,
    0
  );

  return token_ptr;
}

struct lex_token_t *tokenize_char(struct char_stream *stream)
{
  char buf[32];

  int read = stream->api->read_cur(stream->self, buf);
  
  if (read == 0) { return NULL; }
  buf[1] = '\0';

  struct lex_token_t *token_ptr = create_lex_token(
    get_token_type_from_text(buf),
    buf,
    1,
    NULL,
    0
  );
  
  return token_ptr;
}


void tokenize(struct lex_token_list_t *lexer_output, char c, struct char_stream *stream)
{
  struct token_ops ops = TOKEN_REGISTERY[c];
  struct lex_token_t *token_ptr = NULL;

  if (ops.scope_pop) 
  {
    struct scope_t scope_res = {0};
    ops.scope_pop(&scope_stack, &scope_res);
    struct lex_token_t *open_scope_token = get_prev_open_scope_token(lexer_output, scope_res.num_tokens);
    open_scope_token->ctx.num_lines = scope_res.num_lines;
  }

  if (ops.scope_create)
  {
    ops.scope_create(&scope_stack);
  }

  if (ops.tokenize_range) 
  {
    token_ptr = ops.tokenize_range(stream); 
    if (token_ptr != NULL) 
    {
      add_token_to_list(lexer_output, token_ptr);
      if (ops.scope_inc_num_tokens) { ops.scope_inc_num_tokens(&scope_stack); }
    }
  }

  if (ops.tokenize_char)
  {
    token_ptr = ops.tokenize_char(stream);
    if (token_ptr != NULL)
    {
      add_token_to_list(lexer_output, token_ptr);
      if (ops.scope_inc_num_tokens) { ops.scope_inc_num_tokens(&scope_stack); }
    }
    else
    {
      printf("missed char: %c\n", c);
    }
  }

  if (ops.scope_inc_num_lines) { ops.scope_inc_num_lines(&scope_stack); }
}
