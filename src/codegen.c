#include "codegen.h"

#include <string.h>
#include <stdio.h>

static int temp_count = 0;

// Using copy rather than ptrs from func return for simplicity

enum cgen_ret_type {
  CGEN_RET_TYPE_TEMP,
  CGEN_RET_TYPE_TERM,
  CGEN_RET_TYPE_NULL
};

struct cgen_ret_id {
  char text[32];
  size_t text_len;
};

struct cgen_ret {
  enum cgen_ret_type type;
  union {
    int temp;
    struct cgen_ret_id id;
  };
};

void print_cgen_ret(struct cgen_ret *ret)
{
  if (ret->type == CGEN_RET_TYPE_TEMP)
  {
    printf("t%d", ret->temp);
  }
  else
  { 
    printf("%s", ret->id.text);
  }
}

struct cgen_ret cgen_expr_prime(struct ast_expr_prime *expr_prime)
{
  struct cgen_ret ret = {
    .type = CGEN_RET_TYPE_NULL
  };

  if (expr_prime == NULL || expr_prime->type == AST_EXPR_PRIME_TYPE_NULL)
  {
    return ret;
  }

  struct cgen_ret expr_prime_ret = cgen_expr_prime(expr_prime->expr_prime);
  ret.temp = temp_count++;

  if (expr_prime->type == AST_EXPR_PRIME_TYPE_TERM_ONLY)
  {
    ret.type = CGEN_RET_TYPE_TERM;
    memcpy(&ret.id, &expr_prime->term.id, sizeof(struct ast_id));
    return ret;
  }

  ret.type = CGEN_RET_TYPE_TEMP;
  
  if (expr_prime_ret.type == CGEN_RET_TYPE_NULL)
  {
    printf("t%d = %s", ret.temp, expr_prime->term.id.text);
  }
  else
  {
    printf("t%d = %s + ", ret.temp, expr_prime->term.id.text);
    print_cgen_ret(&expr_prime_ret);
  }

  printf("\n");

  return ret;
}

struct cgen_ret cgen_expr(struct ast_expr *expr)
{
  struct cgen_ret ret;
  ret.temp = temp_count++;
  ret.type = CGEN_RET_TYPE_TEMP;
  
  if (expr->expr_prime.type != AST_EXPR_PRIME_TYPE_NULL)
  {
    struct cgen_ret expr_prime_ret = cgen_expr_prime(&expr->expr_prime);
    printf("t%d = %s + ", ret.temp, expr->term.id.text);
    print_cgen_ret(&expr_prime_ret);
    printf("\n");
  }
  else
  { 
    ret.type = CGEN_RET_TYPE_TERM;
    memcpy(&ret.id, &expr->term.id, sizeof(struct ast_id)); 
  }

  return ret;
}

struct cgen_ret cgen_assignment(struct ast_assignment *assign)
{
  struct cgen_ret ret;
  ret.temp = temp_count++;
  struct cgen_ret expr_ret = cgen_expr(&assign->expr);
  printf("%s = ", assign->term.id.text);
  print_cgen_ret(&expr_ret);
  printf("\n");
}

void cgen_line(struct ast_line *line)
{
  // Once other line types are added, do a type check
  cgen_assignment(&line->assignment);
}

void cgen_ast(struct ast_root *root)
{
  for (int i=0; i<root->num_lines; i++)
  {
    cgen_line(&root->lines[i]);
  }
}
