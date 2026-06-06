#pragma once

enum asm_instr {
  ASM_INSTR_NULL,
  ASM_INSTR_MOV,
  ASM_INSTR_ADD,
  ASM_INSTR_CMP,
};

enum asm_register {
  ASM_REGISTER_NULL,
  ASM_REGISTER_RAX
};

char *asm_register_to_text(enum asm_register reg)
{
  switch(reg)
  {
    case ASM_REGISTER_RAX:
      return "rax";
  }
}

char *asm_instr_to_text(enum asm_instr instr)
{
  switch (instr)
  {
    case ASM_INSTR_MOV:
      return "mov";
    case ASM_INSTR_CMP:
      return "cmp";
    case ASM_INSTR_ADD:
      return "add";
  }
}
