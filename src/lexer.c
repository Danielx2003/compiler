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
  if (strcmp(buf, ">") == 0) { return LEX_TOKEN_GREATER_THAN; }
  if (strcmp(buf, "<") == 0) { return LEX_TOKEN_GREATER_THAN; }
  if (strcmp(buf, ")") == 0) { return LEX_TOKEN_CLOSE_BRACKET; }
  if (strcmp(buf, "(") == 0) { return LEX_TOKEN_OPEN_BRACKET; }
  if (strcmp(buf, "+") == 0) { return LEX_TOKEN_ADD; }
  if (strcmp(buf, "=") == 0) { return LEX_TOKEN_EQUAL; }
  if (strcmp(buf, "-") == 0) { return LEX_TOKEN_SUB; }


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

struct scope_t {
  size_t num_lines;
  int num_tokens;
};

struct scope_stack_t {
  struct scope_t *scopes;
  int top;
  int num_scopes;
};

struct scope_t *pop_scope(struct scope_stack_t *stack)
{
  if (stack->top == 0)
  {
    printf("Stack Empty\n");
    return NULL;
  }

  struct scope_t *scope_res = (struct scope_t*)malloc(sizeof(struct scope_t));

  memcpy(scope_res, &stack->scopes[stack->top], sizeof(struct scope_t));
  memset(&stack->scopes[stack->top], 0, sizeof(struct scope_t));
  stack->top--;
  stack->scopes[stack->top].num_lines++;
  stack->scopes[stack->top].num_tokens += scope_res->num_tokens;
  return scope_res;
}

void create_new_scope(struct scope_stack_t *stack)
{
  if (stack->top+1 >= stack->num_scopes)
  {
    stack->scopes = (struct scope_t*)realloc(stack->scopes, sizeof(struct scope_t) * stack->num_scopes*2);

    if (stack->scopes == NULL)
    {
      printf("failed\n");
      return;
    }
    stack->num_scopes = stack->num_scopes * 2;
  }

  stack->top++;
  memset(&stack->scopes[stack->top], 0, sizeof(struct scope_t));
}

struct lex_token_t *create_lex_token(enum lex_token_type type, char *text, size_t text_len, void *ctx, size_t ctx_size)
{
  struct lex_token_t *token = (struct lex_token_t *)calloc(1, sizeof(struct lex_token_t));
  
  // Type
  token->type = type;
  
  // Text
  strcpy(token->text, text);

  // Text Len
  token->text_len = text_len;

  // Ctx
  if (ctx != NULL)
  {
    memcpy(&token->ctx, ctx, ctx_size);
  }

  return token;
}
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

/*

enum lex_token_type
struct token_ops {
  void (*increase_num_lines)(void);
  void (*tokenize_char)();
  void (*tokenize_range)();
}
*/

struct token_ops {
  void (*tokenize_char)(int, FILE);
  void (*tokenize_range)(int, int, FILE);
};

/*

      case '>':
      case '<':
      case '(':
      case ')':
      case '+':
      case '=':
      case '-':
      case '}':
      case '{':
      case ' ':
      case '\n':
      case ';':


*/

const struct token_ops TOKEN_REGISTERY[LEX_TOKEN_EOF] = {
  [LEX_TOKEN_TYPE] = NULL,
  [LEX_TOKEN_ID] = NULL,
  [LEX_TOKEN_SEMI_COLON] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_WHITESPACE] = { .tokenize_range = tokenize_range },
  [LEX_TOKEN_RETURN] = NULL,
  [LEX_TOKEN_CONSTANT] = NULL,
  [LEX_TOKEN_ADD] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_SUB] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_INT] = NULL,
  [LEX_TOKEN_EQUAL] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_IF] = NULL,
  [LEX_TOKEN_OPEN_SCOPE] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_CLOSE_SCOPE] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_OPEN_BRACKET] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_CLOSE_BRACKET] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_EQUIV] = NULL,
  [LEX_TOKEN_LESS_THAN] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_GREATER_THAN] = { .tokenize_char = tokenize_char, .tokenize_range = tokenize_range },
  [LEX_TOKEN_EOF] = NULL
};

void tokenize(char c)
{
  printf("tokenize\n");
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
  
  struct scope_stack_t scope_stack = {0};
  scope_stack.scopes = (struct scope_t*)calloc(10, sizeof(struct scope_t));
  scope_stack.num_scopes = 10;
  scope_stack.top = 0;

  lexer_output->tokens = (struct lex_token_t*)calloc(lexer_output->total_tokens, sizeof(struct lex_token_t));
  lexer_output->cur_idx = 0;

  char buf[32];
  struct lex_token_t *token_ptr = NULL;

  do
  {
    c = (char)fgetc(file);
    
    switch(c)
    {
      case '>':
      case '<':
      case '(':
      case ')':
      case '+':
      case '=':
      case '-':
        if (cur-strt > 0)
        {
          token_ptr = tokenize_span(cur, strt, file);
          add_token_to_list(lexer_output, token_ptr);
          scope_stack.scopes[scope_stack.top].num_tokens++;
        }

        token_ptr = tokenize_char(cur, file);
        add_token_to_list(lexer_output, token_ptr);
        scope_stack.scopes[scope_stack.top].num_tokens++;

        strt = cur+1;
        break;
      case '}':
        if (cur-strt > 0)
        {
          token_ptr = tokenize_span(cur, strt, file);
          add_token_to_list(lexer_output, token_ptr);
          scope_stack.scopes[scope_stack.top].num_tokens++;
        }

        token_ptr = tokenize_char(cur, file);

        struct scope_t *scope_res = pop_scope(&scope_stack);
        
        struct lex_token_t *open_scope_token = get_prev_open_scope_token(lexer_output, scope_res->num_tokens);
        open_scope_token->ctx.num_lines = scope_res->num_lines;
        free(scope_res);

        add_token_to_list(lexer_output, token_ptr);
        scope_stack.scopes[scope_stack.top].num_tokens++;
        
        strt = cur+1;
        break;
      case '{':
        if (cur-strt > 0)
        {
          token_ptr = tokenize_span(cur, strt, file);
          add_token_to_list(lexer_output, token_ptr);
          scope_stack.scopes[scope_stack.top].num_tokens++;
        }

        token_ptr = tokenize_char(cur, file);
        create_new_scope(&scope_stack);
        add_token_to_list(lexer_output, token_ptr);
        scope_stack.scopes[scope_stack.top].num_tokens++;

        strt = cur+1;
        break;
      case ' ':
      case '\n':
        if (cur-strt > 0)
        {
          token_ptr = tokenize_span(cur, strt, file);
          add_token_to_list(lexer_output, token_ptr);
          scope_stack.scopes[scope_stack.top].num_tokens++;
        }

        strt = cur+1;
        break;
      case ';':
        scope_stack.scopes[scope_stack.top].num_lines++;
        if (cur-strt > 0)
        {
          token_ptr = tokenize_span(cur, strt, file);
          add_token_to_list(lexer_output, token_ptr);
          scope_stack.scopes[scope_stack.top].num_tokens++;
        }

        token_ptr = tokenize_char(cur, file);
        add_token_to_list(lexer_output, token_ptr);
        scope_stack.scopes[scope_stack.top].num_tokens++;

        strt = cur+1;
        break;
      default:
        break;
    }
    cur++;

  } while (c != EOF);
  printf("\n");

  token.type = LEX_TOKEN_EOF;
  add_token_to_list(lexer_output, &token);

  if (num_lines == 0) { num_lines++; } 

  return scope_stack.scopes[scope_stack.top].num_lines;
}
