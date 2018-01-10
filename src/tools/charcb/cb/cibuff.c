//
// Created by Enrico on 05.01.18.
//

#include "cibuff.h"

typedef enum _state {
    BEGIN, HAS_NEXT, NEXT_NOT_GUARANTEED
} STATE;

typedef struct _circularbuffer {
    void *buffer;       // pointer to buffer
    size_t capacity;    // maximum number of items in the buffer [1;n]
    size_t elements;    // number of items in the buffer [0;n-1]
    size_t fi_index;    // First-In array index [0;(n-1)] (first element to be read)
    size_t cu_index;    // Cursor array index [0;(n-1)]
    size_t sz;          // size of each item in the buffer
    STATE status;
} CircularBuffer;

CircularBuffer *cb_init(size_t capacity, size_t sz) {
    if (capacity <= 0) {
        fprintf(stderr, "Error: CircularBuffer must have at least 1 element");
        exit(1);
    }

    CircularBuffer *cb;
    cb = malloc(sizeof(struct _circularbuffer));
    // Alloco la memoria necessaria per contenere l'array di elementi
    cb->buffer = malloc(capacity * sz);

    if (cb->buffer == NULL) {
        fprintf(stderr, "Error: circular buffer == NULL");
        exit(1);
    }
    cb->capacity = capacity;
    cb->elements = 0;
    cb->fi_index = 0;
    cb->cu_index = 0;
    cb->sz = sz;
    cb->status = BEGIN;
    return cb;
}

bool cb_push(const void *newitem, CircularBuffer *cb) {
    // Nel caso il buffer non sia pieno e quindi non sovrascriviamo nessun carattere
    if (cb->elements < cb->capacity) {
        memcpy(&(cb->buffer[cb->elements]), newitem, cb->sz);
        cb->elements++;
        return 0;
    }

    // Nel caso il buffer sia pieno e quindi sovrascriviamo il FI
    memcpy(&(cb->buffer[cb->fi_index]), newitem, cb->sz);

    // Se la posizione del cursore è dove sta il FI allora incrementiamo anche il cursore.
    if (cb->cu_index == cb->fi_index) {
        cb->cu_index = (cb->fi_index + 1) % cb->capacity;
        cb->status = BEGIN;
    }

    // Aggiorniamo FI
    cb->fi_index = ++cb->fi_index % cb->capacity;
    return 1;
}

bool cb_hasnext(CircularBuffer *cb) {
    if (cb->status == BEGIN) {
        if (cb->elements == 0) {
            cb->status = NEXT_NOT_GUARANTEED;
            return false;
        }
        return true;
    }
    // Se il prossimo elemento che leggo è il primo ad essere inserito (e quindi il primo a venir letto) allora devo
    // riportare di non aver un prossimo elemento da leggere poichè sono arrivato all'ultimo.
    if ((cb->cu_index + 1) % cb->elements == cb->fi_index) {
        cb->status = NEXT_NOT_GUARANTEED;




        return false;
    }
    cb->status = HAS_NEXT;
    return true;
}

/**
 * Questa funzione ritorna l'id associato all'elemento puntato dal cursore, partendo dal First In al quale si associa 0
 * fino all'ultimo elemento inserito al quale si associa (numero di elementi contenuti - 1)
 * @param cb
 * @return id associato all'elemento puntato dal cursore
 */
size_t cb_getid(CircularBuffer *cb) {
    //Calcolo quanto è distante l'elemento puntato dal primo elemento inserito
    long distance = cb->fi_index - cb->cu_index;
    if (distance <= 0)distance += cb->elements;
    return (size_t) (cb->elements - distance);
}

size_t cb_next(void *item, CircularBuffer *cb) {
    if (cb->status == NEXT_NOT_GUARANTEED) {
        fprintf(stderr, "Error: FIFO -> End of buffer\n");
        exit(1);
    }
    if (cb->status != BEGIN) cb->cu_index = ++cb->cu_index % cb->elements;

    memcpy(item, &(cb->buffer[cb->cu_index]), cb->sz);
    size_t elementID = cb_getid(cb);

    cb->status = NEXT_NOT_GUARANTEED;
    return elementID;
}

size_t cb_pointed(void *item, CircularBuffer *cb) {
    if (cb->status == NEXT_NOT_GUARANTEED) {
        fprintf(stderr, "Error: FIFO -> End of buffer\n");
        exit(1);
    }
    if (cb->status != BEGIN) memcpy(item, &(cb->buffer[(cb->cu_index + 1) % cb->elements]), cb->sz);
    else
        memcpy(item, &(cb->buffer[cb->cu_index]), cb->sz);
    return cb_getid(cb);
}

size_t cb_nofel(CircularBuffer *cb) {
    return cb->elements;
}

void cb_read(size_t id, void *item, CircularBuffer *cb) {
    if (id >= cb->capacity) {
        fprintf(stderr, "Error: Circular Buffer -> Out of bounds\n - cb_read(%zu)\n", id);
        exit(22);
    }
    cb_setid(id, cb);
    cb->cu_index = (cb->fi_index + id) % cb->elements;
    memcpy(item, &(cb->buffer[cb->cu_index]), cb->sz);
}

void cb_setid(size_t id, CircularBuffer *cb) {
    if (id >= cb->capacity) {
        fprintf(stderr, "Error: Circular Buffer -> Out of bounds\n - cb_setid(%zu)\n", id);
        exit(22);
    }
    cb->status = NEXT_NOT_GUARANTEED;
    if (id == 0) {
        return cb_reset(cb);
    }
    cb->cu_index = (cb->fi_index + id - 1) % cb->elements;
}

void cb_reset(CircularBuffer *cb) {
    cb->cu_index = cb->fi_index;
    cb->status = BEGIN;
}

void cb_free(CircularBuffer *cb) {
    free(cb->buffer);
}