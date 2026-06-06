#include "codegen.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int offset = 8;
int OFFSETS[256];
FILE *file;

void generate_goto(struct ir_goto *go_to)
{
  fprintf(file, "jmp L%d\n", go_to->label);
}

void generate_conditional(struct ir_conditional *cond)
{
  fprintf(file, "jne L%d\n", cond->label);  
}

void generate_label(struct ir_label *label)
{
  fprintf(file, "L%d:\n", label->label);
}

void generate_assign(struct ir_assign *assign)
{
  // if no rhs_2 (i.e. no operation)
  if (assign->op == IR_ASSIGN_OP_NONE)
  {
    char buf[100];
    if (assign->rhs_1.type == IR_TERM_ID)
    {
      fprintf(file, "mov rax, [rsp - %d]\n", OFFSETS[assign->rhs_1.text[0]]);
      sprintf(buf, "rax");
    }
    else if (assign->rhs_1.type == IR_TERM_TEMP)
    {
      fprintf(file, "mov rax, [rsp - %d]\n", OFFSETS[assign->rhs_1.temp]);

    }
    else if (assign->rhs_1.type == IR_TERM_CONSTANT)
    {
      fprintf(file, "mov rax, %d\n", assign->rhs_1.constant);
    }
    
    if (assign->lhs.type == IR_TERM_TEMP)
    {
      if (OFFSETS[assign->lhs.temp] == 0)
      {
        printf("set: t%d = %d\n", assign->lhs.temp, offset);
        OFFSETS[assign->lhs.temp] = offset;
        offset += 8;
      }
      fprintf(file, "mov [rsp - %d], rax\n", OFFSETS[assign->lhs.temp]);
    }
    else if (assign->lhs.type == IR_TERM_ID)
    {
      if (OFFSETS[assign->lhs.text[0]] == 0)
      {
        printf("set: %s = %d\n", assign->lhs.text, offset);
        OFFSETS[assign->lhs.text[0]] = offset;
        offset += 8;
      }
      fprintf(file, "mov [rsp - %d], rax\n", OFFSETS[assign->lhs.text[0]]);
    }
  }
  else if (assign->op == IR_ASSIGN_OP_ADD)
  {
    if (
      assign->rhs_2.type == IR_TERM_ID
    )
    {
      fprintf(file, "mov rax, [rsp - %d]\n", OFFSETS[assign->rhs_2.text[0]]);
    }
    else if (assign->rhs_2.type == IR_TERM_TEMP)
    {
      fprintf(file, "mov rax, [rsp - %d]\n", OFFSETS[assign->rhs_2.temp]);
    }
    else 
    {
      fprintf(file, "mov rax, %d\n", assign->rhs_2.constant);
    }
 
    if (
      assign->rhs_1.type == IR_TERM_ID
    )
    {
      fprintf(file, "add rax, [rsp - %d]\n", OFFSETS[assign->rhs_1.text[0]]);
    }
    else if (assign->rhs_1.type == IR_TERM_TEMP)
    {
      fprintf(file, "add rax, [rsp - %d]\n", OFFSETS[assign->rhs_1.temp]);
    }
    else 
    {
      fprintf(file, "add rax, %d\n", assign->rhs_1.constant);
    }

    if (assign->lhs.type == IR_TERM_TEMP)
    {
      if (OFFSETS[assign->lhs.temp] == 0)
      {
        printf("set: t%d = %d\n", assign->lhs.temp, offset);
        OFFSETS[assign->lhs.temp] = offset;
        offset += 8;
      }

      fprintf(file, "mov [rsp - %d], rax\n", OFFSETS[assign->lhs.temp]);
    }
    else if (assign->lhs.type == IR_TERM_ID)
    {
      if (OFFSETS[assign->lhs.text[0]] == 0)
      {
        printf("set: %s = %d\n", assign->lhs.text, offset);
        OFFSETS[assign->lhs.text[0]] = offset;
        offset += 8;
      }
      fprintf(file, "mov [rsp - %d], rax (lhs = id)\n", OFFSETS[assign->lhs.text[0]]);
    }

  }
  else if (assign->op == IR_ASSIGN_OP_EQUIV)
  {
    if (
      assign->rhs_2.type == IR_TERM_ID
    )
    {
      fprintf(file, "mov rax, [rsp - %d]\n", OFFSETS[assign->rhs_2.text[0]]);
    }
    else if (assign->rhs_2.type == IR_TERM_TEMP)
    {
      fprintf(file, "mov rax, [rsp - %d]\n", OFFSETS[assign->rhs_2.temp]);
    }
    else 
    {
      fprintf(file, "mov rax, %d\n", assign->rhs_2.constant);
    }
 
    if (
      assign->rhs_1.type == IR_TERM_ID
    )
    {
      fprintf(file, "cmp rax, [rsp - %d]\n", OFFSETS[assign->rhs_1.text[0]]);
    }
    else if (assign->rhs_1.type == IR_TERM_TEMP)
    {
      fprintf(file, "cmp rax, [rsp - %d] (rhs_1.temp)\n", OFFSETS[assign->rhs_1.temp]);
    }
    else 
    {
      fprintf(file, "cmp rax, %d\n", assign->rhs_1.constant);
    }
  }
  
}

void generate_item(struct ir_item *item)
{
  switch(item->type)
  {
    case IR_ITEM_ASSIGN:
      generate_assign(&item->assign);
      break;
    case IR_ITEM_LABEL:
      generate_label(&item->label);
      break;
    case IR_ITEM_CONDITIONAL:
      generate_conditional(&item->conditional);
      break;
    case IR_ITEM_GOTO:
      generate_goto(&item->go_to);
  }
}

void generate_yasm(struct ir_stream *stream)
{
  file = fopen("test.asm", "w");
  if (file == NULL)
  {
    printf("failed to open test.asm\n");
    return;
  }
  
  char *str = "global _start\nsection .data\nsection .text\n_start:\npush rbp\nmov rbp, rsp\nsub rsp, 32\n";
  fwrite(str, sizeof(char), strlen(str), file);

  for (int i = 0; i < stream->total; i++)
  {
    generate_item(&stream->items[i]);
  }

  str = "mov rax, 60\nsyscall\n";
  fwrite(str, sizeof(char), strlen(str), file);

  fclose(file);
}
