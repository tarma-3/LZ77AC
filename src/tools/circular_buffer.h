//
// Created by Enrico on 06.12.16.
//

#ifndef LISTACIRCOLARE_CIRCULAR_BUFFER_H
#define LISTACIRCOLARE_CIRCULAR_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct circular_buffer circular_buffer;

void cb_free(circular_buffer *cb);

int cb_push(const void *item,circular_buffer *cb);
int cb_delpush(const void *item, circular_buffer *cb);
int cb_pop(void *item,circular_buffer *cb);

circular_buffer *cb_init(size_t capacity, size_t sz);
int cb_errorcode(circular_buffer *cb);

#endif //LISTACIRCOLARE_CIRCULAR_BUFFER_H