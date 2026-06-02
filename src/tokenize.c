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

struct lex_token_t *tokenize_span(int cur, int strt, FILE *file)
{
  char buf[32];

  fseek(file, strt, SEEK_SET);
  fread(buf, sizeof(char), cur-strt, file);
  fseek(file, cur+1, SEEK_SET);
    
  buf[cur-strt] = '\0';

  struct lex_token_t *token_ptr = create_lex_token(
    get_token_type_from_text(buf),
    buf,
    cur-strt,
    NULL,
    0
  );

  return token_ptr;
}

struct lex_token_t *tokenize_char(int cur, FILE *file)
{
  char buf[32];

  fseek(file, cur, SEEK_SET);
  fread(buf, sizeof(char), 1, file);
  fseek(file, cur+1, SEEK_SET);

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


void tokenize(struct lex_token_list_t *lexer_output, char c, int cur, int strt, FILE *file)
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
    if (cur-strt > 0)
    {
      token_ptr = ops.tokenize_range(cur, strt, file); 
      add_token_to_list(lexer_output, token_ptr);
      if (ops.scope_inc_num_tokens) { ops.scope_inc_num_tokens(&scope_stack); }
      else { printf("dint inc num line\n"); }
    }
  }

  if (ops.tokenize_char)
  {
    token_ptr = ops.tokenize_char(cur, file);
    add_token_to_list(lexer_output, token_ptr);
    if (ops.scope_inc_num_tokens) { ops.scope_inc_num_tokens(&scope_stack); }
    else { printf("dint inc num line\n"); }
  }

  if (ops.scope_inc_num_lines) { ops.scope_inc_num_lines(&scope_stack); }
}
