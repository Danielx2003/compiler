#include "lexer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void add_token_to_list(struct token_list_t *lexer_output, struct token_t *new_token)
{
  if (lexer_output->cur_idx >= lexer_output->total_tokens) // Increase size
  {
    lexer_output->tokens = realloc(lexer_output->tokens, lexer_output->total_tokens * 2);
  }

  memcpy(
      &lexer_output->tokens[lexer_output->cur_idx], 
      new_token,
      sizeof(struct token_t)
  );
  memset(new_token, 0, sizeof(struct token_t));

  lexer_output->cur_idx += 1;
}

static enum token_type get_token_type_from_text(char *buf)
{
  if (strcmp(buf, "return") == 0) { return TOKEN_TYPE_RETURN; }

  switch(buf[0])
  {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
      return TOKEN_TYPE_CONSTANT; 
    case '+':
      return TOKEN_TYPE_ADD;
    case '-':
      return TOKEN_TYPE_SUB;
  }

  return TOKEN_TYPE_ID;  
}

struct token_t* tokenize_stream(
    struct token_list_t *lexer_output,
    char *input,
    size_t input_size,
    size_t *token_size
)
{
  int strt = 0;
  int prev = 0;
  int cur = 0;
  char *text;
  struct token_t token = {0};

  lexer_output->tokens = (struct token_t*)calloc(16, sizeof(struct token_t));
  lexer_output->total_tokens = 16;
  lexer_output->cur_idx = 0;


  /* 5 + 4+3;*/
  /*
  
  5 - strt = 0, cur = 0, prev = ?
  SPACE - strt = 0, cur = 1, prev = 0? -> results in no buffer flush
  4 - strt = 2, cur = 2, prev = 1
  + - strt = 2, cur = 3, prev = 2 -> no buffer flush

   */

  while (cur < input_size)
  {
    switch(input[cur])
    {
      case '+':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_ADD;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case ' ':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        strt = cur+1;
        break;
      case ';':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_SEMI_COLON;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      default:
       break; 
    }
    prev = cur;
    cur++;
  }

  token.type = TOKEN_TYPE_EOF;
  add_token_to_list(lexer_output, &token);

  return lexer_output->tokens;
}
