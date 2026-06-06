#include "lexer.h"
#include "scope_2.h"
#include "tokenize.h"
#include "file_stream.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* Globals */
int num_lines = 0;
char c;
struct lex_token_t token = {0};
struct scope_stack_t scope_stack = {0};


void add_token_to_list(struct lex_token_stream *tokens, struct lex_token_t *new_token)
{
  if (tokens->cur_idx >= tokens->capacity) // Increase size
  {
    tokens->capacity = tokens->capacity * 2;
    tokens->data= realloc(tokens->data, tokens->capacity + 1);
  }

  memcpy(
      &tokens->data[tokens->cur_idx], 
      new_token,
      sizeof(struct lex_token_t)
  );
  memset(new_token, 0, sizeof(struct lex_token_t));

  tokens->cur_idx += 1;
}

enum lex_token_type get_token_type_from_text(char *buf)
{
  if (strcmp(buf, "return") == 0) { return LEX_TOKEN_RETURN; }
  if (strcmp(buf, "int") == 0) { return LEX_TOKEN_INT; }
  if (strcmp(buf, "if") == 0) { return LEX_TOKEN_IF; }
  if (strcmp(buf, "while") == 0) { return LEX_TOKEN_WHILE; }
  if (strcmp(buf, "==") == 0) { return LEX_TOKEN_EQUIV; }
  if (strcmp(buf, ">") == 0) { return LEX_TOKEN_GREATER_THAN; }
  if (strcmp(buf, "<") == 0) { return LEX_TOKEN_GREATER_THAN; }
  if (strcmp(buf, ")") == 0) { return LEX_TOKEN_CLOSE_BRACKET; }
  if (strcmp(buf, "(") == 0) { return LEX_TOKEN_OPEN_BRACKET; }
  if (strcmp(buf, "+") == 0) { return LEX_TOKEN_ADD; }
  if (strcmp(buf, "=") == 0) { return LEX_TOKEN_EQUAL; }
  if (strcmp(buf, "-") == 0) { return LEX_TOKEN_SUB; }
  if (strcmp(buf, ";") == 0) { return LEX_TOKEN_SEMI_COLON; }
  if (strcmp(buf, "}") == 0) { return LEX_TOKEN_CLOSE_SCOPE; }
  if (strcmp(buf, "{") == 0) { return LEX_TOKEN_OPEN_SCOPE; }

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
      return LEX_TOKEN_CONSTANT; 
  }

  return LEX_TOKEN_ID;  
}

struct lex_token_t* get_prev_open_scope_token(struct lex_token_stream *tokens, int tokens_since)
{
  if (tokens->cur_idx - tokens_since < 0)
  {
    return NULL;
  }
  
  return &tokens->data[tokens->cur_idx - tokens_since];
}

struct lex_token_t *create_lex_token(enum lex_token_type type, char *text, size_t text_len, void *ctx, size_t ctx_size)
{
  struct lex_token_t *token = (struct lex_token_t *)calloc(1, sizeof(struct lex_token_t)); 
  token->type = type;
  strcpy(token->text, text);
  token->text_len = text_len;
  if (ctx != NULL) { memcpy(&token->ctx, ctx, ctx_size); }

  return token;
}

int lex_tokenize_stream(
  struct lex_token_stream *tokens,
  struct char_stream *stream
)
{  
  init_scope_stack(&scope_stack);

  tokens->data = (struct lex_token_t*)calloc(tokens->capacity, sizeof(struct lex_token_t));
  tokens->cur_idx = 0;

  do
  {
    c = stream->api->consume(stream->self);
    
    switch(c)
    {
      case '>':
      case '<':
      case '(':
      case ')':
      case ' ':
      case '\n':
      case ';':
      case '}':
      case '{':
        tokenize(tokens, c, stream);

        // start = cur
        stream->api->reset_window(
          stream->self
        );

        stream->api->advance_start(
          stream->self, 
          1
        );

        break;
      default:
        break;
    }
    stream->api->advance_cur(
      stream->self,
      1
    );

  } while (c != EOF);
  printf("\n");

  token.type = LEX_TOKEN_EOF;
  add_token_to_list(tokens, &token);

  if (num_lines == 0) { num_lines++; } 

  return scope_stack.scopes[scope_stack.top].num_lines;
}
