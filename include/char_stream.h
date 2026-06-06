#pragma once

struct char_stream_api {
  char (*consume)(void *self);
  int (*read)(void *self, char *buf, int size);
  int (*read_prev)(void *self, char *buf, int size);
  int (*read_cur)(void *self, char *buf);

  int (*get_window_size)(void *self);

  void (*advance_cur)(void *self, int amount);
  void (*advance_start)(void *self, int amount);
  void (*reset_window)(void *self);
};

struct char_stream {
  void *self;
  const struct char_stream_api *api;
};
