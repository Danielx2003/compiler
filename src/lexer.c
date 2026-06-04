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


void add_token_to_list(struct lex_token_list_t *lexer_output, struct lex_token_t *new_token)
{
  if (lexer_output->cur_idx >= lexer_output->total_tokens) // Increase size
  {
    lexer_output->total_tokens = lexer_output->total_tokens * 2;
    lexer_output->tokens = realloc(lexer_output->tokens, lexer_output->total_tokens + 1);
  }

  memcpy(
      &lexer_output->tokens[lexer_output->cur_idx], 
      new_token,
      sizeof(struct lex_token_t)
  );
  memset(new_token, 0, sizeof(struct lex_token_t));

  lexer_output->cur_idx += 1;
}

enum lex_token_type get_token_type_from_text(char *buf)
{
  if (strcmp(buf, "return") == 0) { return LEX_TOKEN_RETURN; }
  if (strcmp(buf, "int") == 0) { return LEX_TOKEN_INT; }
  if (strcmp(buf, "if") == 0) { return LEX_TOKEN_IF; }
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

struct lex_token_t* get_prev_open_scope_token(struct lex_token_list_t *lexer_output, int tokens_since)
{
  if (lexer_output->cur_idx - tokens_since < 0)
  {
    return NULL;
  }
  
  return &lexer_output->tokens[lexer_output->cur_idx - tokens_since];
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
    struct lex_token_list_t *lexer_output,
    char *input,
    size_t input_size,
    size_t *token_size
)
{
  /*
  char *file_name = "test.yega";
  struct file_iterator *it = create_file_iterator(file_name);
  */

  FILE *file;
  file = fopen("test.yega", "r");
  if (file == NULL)
  {
    printf("Failed to open file\n");
    return 0;
  }

  struct file_stream_state file_state = {0};
  struct char_stream file_stream = char_stream_from_file(&file_state, file);
  
  init_scope_stack(&scope_stack);

  lexer_output->tokens = (struct lex_token_t*)calloc(lexer_output->total_tokens, sizeof(struct lex_token_t));
  lexer_output->cur_idx = 0;

  struct lex_token_t *token_ptr = NULL;

  do
  {
    // c = (char)fgetc(file);
    c = file_stream.api->consume(file_stream.self);
    
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
        tokenize(lexer_output, c, &file_stream);

        file_stream.api->reset_window(
          file_stream.self
        );

        file_stream.api->advance_start(
          file_stream.self, 
          1
        );

        break;
      default:
        break;
    }
    file_stream.api->advance_cur(
      file_stream.self,
      1
    );

  } while (c != EOF);
  printf("\n");

  token.type = LEX_TOKEN_EOF;
  add_token_to_list(lexer_output, &token);

  if (num_lines == 0) { num_lines++; } 

  return scope_stack.scopes[scope_stack.top].num_lines;
}
