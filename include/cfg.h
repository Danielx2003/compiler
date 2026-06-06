#pragma once

#include "ir.h"

/*
if - then - else:

    if
then  else
   join


while:
-> condition -> |
|               |
|               |
<- loop         |
                |
 join  <---------

*/

struct basic_block {
  struct ir_item leader;
  struct ir_item exit;
  struct basic_block *prev;
  struct basic_block *next;
};

void cfg_walk_ir(struct ir_stream *stream);
