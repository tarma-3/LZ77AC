#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
typedef struct _circularbuffer CircularBuffer;

// "Constructors & destructors"
void cb_free(CircularBuffer *cb);
CircularBuffer *cb_init(size_t capacity, size_t sz);

// Funzioni di inserimento dati
bool cb_push(const void *item,CircularBuffer *cb);

// Funzioni di recupero dati
void cb_read(size_t id, void *item, CircularBuffer *cb);
size_t cb_next(void *item, CircularBuffer *cb);
size_t cb_pointed(void *item, CircularBuffer *cb);

size_t cb_getid(CircularBuffer *cb);
void cb_setid(size_t id, CircularBuffer *cb);
void cb_reset(CircularBuffer *cb);

size_t cb_nofel(CircularBuffer *cb);

bool cb_hasnext(CircularBuffer *cb);