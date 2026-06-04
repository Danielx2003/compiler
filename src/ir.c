#include "ir.h"

#include <string.h>
#include <stdio.h>

static int temp_count = 0;
static int label_count = 0;

// struct ir_item ir_list[32] = {0};
static int ir_idx = 0;

void ir_ast(struct ast_body *root)
{
  // printf("%d Lines: \n", root->num_lines);
  for (int i=0; i<root->num_lines; i++)
  {
    // ir_line(&root->lines[i]);
  }
}
