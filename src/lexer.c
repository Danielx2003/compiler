#include "lexer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void add_token_to_list(struct lex_token_list_t *lexer_output, struct lex_token_t *new_token)
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

static enum lex_token_type get_token_type_from_text(char *buf)
{
  if (strcmp(buf, "return") == 0) { return LEX_TOKEN_RETURN; }
  if (strcmp(buf, "int") == 0) { return LEX_TOKEN_INT; }
  if (strcmp(buf, "if") == 0) { return LEX_TOKEN_IF; }
  if (strcmp(buf, "==") == 0) { return LEX_TOKEN_EQUIV; }

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
    case '+':
      return LEX_TOKEN_ADD;
    case '-':
      return LEX_TOKEN_SUB;
    case '=':
      return LEX_TOKEN_EQUAL;
  }

  return LEX_TOKEN_ID;  
}

int lex_tokenize_stream(
    struct lex_token_list_t *lexer_output,
    char *input,
    size_t input_size,
    size_t *token_size
)
{
  FILE *file;
  file = fopen("test.yega", "r");
  if (file == NULL)
  {
    printf("Failed to open file\n");
    return 0;
  }


  int strt = 0;
  int num_lines = 0;
  int cur = 0;
  char c;
  struct lex_token_t token = {0};

  lexer_output->tokens = (struct lex_token_t*)calloc(lexer_output->total_tokens, sizeof(struct lex_token_t));
  lexer_output->cur_idx = 0;

  do
  {
    c = (char)fgetc(file);
    
    switch(c)
    {
      case '>':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_GREATER_THAN;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '<':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_LESS_THAN;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '}':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_CLOSE_SCOPE;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '{':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_OPEN_SCOPE;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case ')':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_CLOSE_BRACKET;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '(':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_OPEN_BRACKET;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '+':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = LEX_TOKEN_ADD;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case ' ':
      case '\n':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        strt = cur+1;
        break;
      case ';':
        num_lines++;
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          fseek(file, strt, SEEK_SET);
          fread(token.text, sizeof(char), cur-strt, file);
          fseek(file, cur+1, SEEK_SET);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }
        

        token.type = LEX_TOKEN_SEMI_COLON;
        token.text_len = 1;
        fseek(file, cur, SEEK_SET);
        fread(token.text, sizeof(char), 1, file);
        fseek(file, cur+1, SEEK_SET);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      default:
       break; 
    }
    cur++;

  } while (c != EOF);
  printf("\n");

  // replace with consuming token from files, as files may be too big for a single buffer

  /*
  while (cur < input_size)
  {
    switch(input[cur])
    {
      case '>':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_GREATER_THAN;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '<':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_LESS_THAN;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '}':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_CLOSE_SCOPE;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '{':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_OPEN_SCOPE;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case ')':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_CLOSE_BRACKET;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
      case '(':
        if (cur-strt > 0)
        {
          token.text_len = cur-strt;
          memcpy(token.text, &input[strt], cur-strt);
          token.type = get_token_type_from_text(token.text);

          add_token_to_list(lexer_output, &token);
        }

        token.type = TOKEN_TYPE_OPEN_BRACKET;
        token.text_len = 1;
        memcpy(token.text, &input[cur], 1);
        add_token_to_list(lexer_output, &token);

        strt = cur+1;
        break;
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

          if (token.type == TOKEN_TYPE_CONSTANT)
          {
            printf("constatn: %s\n", token.text);
          }

          add_token_to_list(lexer_output, &token);
        }

        strt = cur+1;
        break;
      case ';':
        num_lines++;
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
*/

  token.type = LEX_TOKEN_EOF;
  add_token_to_list(lexer_output, &token);

  if (num_lines == 0) { num_lines++; } 

  return num_lines;
}
