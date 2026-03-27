#include "lexer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// return 42;
// return "hello world"
// RET, SMARK_START, EXPR, SMARK_END

static void add_token_to_list(struct token_list_t *tokens, struct token_t *new_token)
{
  if (tokens->idx >= tokens->len) // Increase size
  {
    tokens->token_list = realloc(tokens->token_list, tokens->len * 2);
  }

  memcpy(
      &tokens->token_list[tokens->idx], 
      new_token, 
      sizeof(struct token_t)
  );

  tokens->idx += 1;
}

static enum token_type get_token_type_from_text(char *buf)
{
  if (strcmp(buf, "return") == 0) { return RETURN; }

  return IDENTIFIER;  
}

struct token_t* tokenize_stream(char *input, size_t input_size, size_t *token_size)
{
  struct token_list_t tokens = {0};
  int strt = 0;
  int prev = 0;
  int cur = 0;
  char *text;
  struct token_t token = {0};

  tokens.token_list = (struct token_t*)calloc(16, sizeof(struct token_t));
  tokens.len = 16;
  tokens.idx = 0;
  
  /*
  do we split on string, or take it char by char?

  char by char:
  - terminate current buffer on deliminator
  - allocate buf space between cur and strt
  - pattern match this token (or switch statement etc)
    - hashmap would be ideal but not in the std lib
   */

  while (cur < input_size)
  {
    switch(input[cur])
    {
      case ' ':
        token.text_len = prev+1-strt;
        memcpy(token.text, &input[strt], prev+1-strt);
        token.type = get_token_type_from_text(token.text);

        add_token_to_list(&tokens, &token);

        memset(&token, 0, sizeof(token));

        strt = cur++;
        break;
      case ';':
        token.text_len = prev+1-strt;
        memcpy(token.text, &input[strt], prev+1-strt);
        token.type = get_token_type_from_text(token.text);

        add_token_to_list(&tokens, &token);

        memset(&token, 0, sizeof(token));

        token.type = SEMI_COLON;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(&tokens, &token);

        memset(&token, 0, sizeof(token));

        strt = cur++;
        break;
      default:
       break; 
    }
    prev = cur;
    cur++;
  }

  
  printf("Extracted Tokens:\n");
  for (int i = 0; i < tokens.idx; i++)
  {
    printf("Token: %d Text: %s\n", tokens.token_list[i].type, tokens.token_list[i].text);
  }
  printf("---\n");

  return tokens.token_list; // update to ret just tokens
}
