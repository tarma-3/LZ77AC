#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
typedef struct fifo fifo;

void fifob_free(fifo *cb);
fifo *fifo_init(size_t capacity, size_t sz);

int fifo_push(const void *item,fifo *fi);
size_t fifo_read(void *item, size_t id, fifo *fi);

size_t fifo_getid(fifo *fi);

size_t fifo_next(void *item, fifo *fi);
bool fifo_hasnext(fifo *fi);

int fifo_errorcode(fifo *cb);