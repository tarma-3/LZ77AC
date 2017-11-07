#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
typedef struct _circularbuffer CircularBuffer;

void cb_free(CircularBuffer *cb);
CircularBuffer *cb_init(size_t capacity, size_t sz);

int cb_push(const void *item,CircularBuffer *cb);
size_t cb_read(void *item, size_t id, CircularBuffer *cb);

size_t cb_getid(CircularBuffer *cb);

size_t cb_next(void *item, CircularBuffer *cb);
bool cb_hasnext(CircularBuffer *cb);

int cb_errorcode(CircularBuffer *cb);