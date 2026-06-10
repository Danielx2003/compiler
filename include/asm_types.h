#pragma once

enum asm_instr {
  ASM_INSTR_NULL,
  ASM_INSTR_MOV,
  ASM_INSTR_ADD,
  ASM_INSTR_CMP,
};

enum asm_register {
  ASM_REGISTER_NULL,
  ASM_REGISTER_RAX,
  ASM_REGISTER_RDI,
  ASM_REGISTER_RSI,
  ASM_REGISTER_RDX,
  ASM_REGISTER_RCX,
  ASM_REGISTER_R8,
  ASM_REGISTER_R9
};

char *asm_register_to_text(enum asm_register reg)
{
  switch(reg)
  {
    case ASM_REGISTER_RAX:
      return "rax";
    case ASM_REGISTER_RDI:
      return "rdi";
    case ASM_REGISTER_RSI:
      return "rsi";
    case ASM_REGISTER_RDX:
      return "rdx";
    case ASM_REGISTER_RCX:
      return "rcx";
    case ASM_REGISTER_R8:
      return "r8";
    case ASM_REGISTER_R9:
      return "r9";
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
