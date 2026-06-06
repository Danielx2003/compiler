#include "codegen.h"
#include "asm_types.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int offset = 8;
int OFFSETS[256];
FILE *file;


char *ir_term_to_register(struct ir_term *term)
{
  static char buf[32];

  if (term->type == IR_TERM_ID)
  {
    sprintf(buf, "[rsp-%d]", OFFSETS[term->text[0]]);
  }
  else if (term->type == IR_TERM_TEMP)
  {
    sprintf(buf, "[rsp-%d]", OFFSETS[term->temp]);
  }
  else if (term->type == IR_TERM_CONSTANT)
  {
    sprintf(buf, "%d", term->constant);
  }

  return buf;
}

void try_register_term(struct ir_term *term)
{
  if (term->type == IR_TERM_TEMP)
  {
    if (OFFSETS[term->temp] == 0)
    {
      printf("set: t%d = %d\n", term->temp, offset);
      OFFSETS[term->temp] = offset;
      offset += 8;
    }
  }
  else if (term->type == IR_TERM_ID)
  {
    if (OFFSETS[term->text[0]] == 0)
    {
      printf("set: %s = %d\n", term->text, offset);
      OFFSETS[term->text[0]] = offset;
      offset += 8;
    }
  }
}

void create_instruction_register_term(enum asm_instr instr, enum asm_register lhs, struct ir_term *rhs)
{
  fprintf(file, "%s %s, %s\n", asm_instr_to_text(instr), asm_register_to_text(lhs), ir_term_to_register(rhs));
}

void create_instruction_term_register(enum asm_instr instr, struct ir_term *lhs, enum asm_register rhs)
{
  fprintf(file, "%s %s, %s\n", asm_instr_to_text(instr), ir_term_to_register(lhs), asm_register_to_text(rhs));
}

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
  if (assign->op == IR_ASSIGN_OP_NONE)
  {
    create_instruction_register_term(ASM_INSTR_MOV, ASM_REGISTER_RAX, &assign->rhs_1);
    try_register_term(&assign->lhs); // will be removed once register alloc is done
    create_instruction_term_register(ASM_INSTR_MOV, &assign->lhs, ASM_REGISTER_RAX);
  }
  else if (assign->op == IR_ASSIGN_OP_ADD)
  {
    create_instruction_register_term(ASM_INSTR_MOV, ASM_REGISTER_RAX, &assign->rhs_2);
    create_instruction_register_term(ASM_INSTR_ADD, ASM_REGISTER_RAX, &assign->rhs_1);

    try_register_term(&assign->lhs); // will be removed once register alloc is done
    create_instruction_term_register(ASM_INSTR_MOV, &assign->lhs, ASM_REGISTER_RAX);
  }
  else if (assign->op == IR_ASSIGN_OP_EQUIV)
  {
    create_instruction_register_term(ASM_INSTR_MOV, ASM_REGISTER_RAX, &assign->rhs_2);
    create_instruction_register_term(ASM_INSTR_CMP, ASM_REGISTER_RAX, &assign->rhs_1);
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
