

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct circular_buffer circular_buffer;

void fifob_free(circular_buffer *cb);

int fifo_push(const void *item,circular_buffer *fi);
int fifo_delpush(const void *item, circular_buffer *cb);
int fifo_pop(void *item,circular_buffer *fi);

circular_buffer *fifo_init(size_t capacity, size_t sz);
int fifo_errorcode(circular_buffer *cb);
size_t fifo_next(void *item, circular_buffer *fi);