#include "file_stream.h"

#include <stdio.h>
#include <stdlib.h>

char file_consume(void *self)
{
  struct file_stream_state *state = (struct file_stream_state *)self;
  return (char)fgetc(state->file);
}

int file_read(void *self, char *buf, int size)
{
  struct file_stream_state *state = (struct file_stream_state *)self;
  return fread(buf, sizeof(char), size, state->file);
}

int file_read_prev(void *self, char *buf, int size)
{
  struct file_stream_state *state = (struct file_stream_state *)self;

  if (state->cur - state->start <= 0 && size > 1) { return 0; }

  fseek(state->file, state->cur-size, SEEK_SET);
  int read = fread(buf, sizeof(char), size, state->file);
  fseek(state->file, state->cur, SEEK_SET);
  
  return read;
}

int file_read_cur(void *self, char *buf)
{
  struct file_stream_state *state = (struct file_stream_state *)self;

  fseek(state->file, state->cur, SEEK_SET);
  int read = fread(buf, sizeof(char), 1, state->file);
  fseek(state->file, state->cur, SEEK_SET);

  return read;
}

int file_get_window_size(void *self)
{
  struct file_stream_state *state = (struct file_stream_state *)self;
  return state->cur - state->start;
}

void file_advance_start(void *self, int amount)
{
  struct file_stream_state *state = (struct file_stream_state *)self;
  state->start += amount;
}

void file_advance_cur(void *self, int amount)
{
  struct file_stream_state *state = (struct file_stream_state *)self;
  state->cur += amount;
  fseek(state->file, state->cur, SEEK_SET);
}

void file_reset_window(void *self)
{
  struct file_stream_state *state = (struct file_stream_state *)self;
  state->start = state->cur;
}

static const struct char_stream_api FILE_STREAM_API = {
  .consume = file_consume,
  .read = file_read,
  .read_prev = file_read_prev,
  .read_cur = file_read_cur,
  .get_window_size = file_get_window_size,
  .advance_cur = file_advance_cur,
  .advance_start = file_advance_start,
  .reset_window = file_reset_window
};

struct char_stream char_stream_from_file(struct file_stream_state *state, FILE *f) {
    state->file = f;
    return (struct char_stream){
        .self = state,
        .api = &FILE_STREAM_API
    };
}

