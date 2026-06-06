#pragma once

#include "char_stream.h"

#include <stdio.h>

struct file_stream_state {
  FILE *file;
  int cur;
  int start;
};

char file_consume(void *self);
int file_read(void *self, char *buf, int size);
int file_read_prev(void *self, char *buf, int size);
struct char_stream char_stream_from_file(struct file_stream_state *state, FILE *fp);
