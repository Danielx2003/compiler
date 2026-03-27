#include "lexer.h"

#include <stdio.h>


int main()
{
  size_t token_list_len;
  tokenize_stream("return 42;", sizeof("return 42;"), &token_list_len); 

  printf("Compiler work starts here\n");
}
