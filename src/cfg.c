#include "cfg.h"

#include <stdio.h>
#include <stdbool.h>

/*
leaders:
- first instruction in program,
- target of any jump instruction
- next instruction after any jump instruction

*/

bool should_create_new_block(struct ir_item *item)
{
  switch(item->type)
  {
    case IR_ITEM_LABEL:
      return true;
  }

  return false;
}

void cfg_walk_ir(struct ir_stream *stream)
{
  for (int i = 0; i < stream->total; i++)
  {
    if (
      stream->items[i].type == IR_ITEM_LABEL
      || (i > 0 && stream->items[i-1].type == IR_ITEM_CONDITIONAL)
    )
    {
      printf("create new block on index: %d\n", i);
    }
  }
}
