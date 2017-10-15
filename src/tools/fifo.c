//
// Created by Enrico on 06.12.16.
//

#include "fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct circular_buffer {
    void *buffer;       // pointer to buffer
    void *buffer_end;   // pointer to buffer last address
    void *head;         // pointer to head
    void *tail;         // pointer to tail
    void *rtail;        // pointer to last read element
    size_t rcoursor;
    size_t capacity;    // maximum number of items in the buffer
    size_t cursor;      // number of items in the buffer
    size_t sz;          // size of each item in the buffer
    int error_code;
};

circular_buffer *fifo_init(size_t capacity, size_t sz) {
    circular_buffer *fi;
    fi = malloc(sizeof(struct circular_buffer));
    // Alloco la memoria necessaria per contenere l'array di elementi
    fi->buffer = malloc(capacity * sz);
    if (fi->buffer == NULL) {
        fprintf(stderr,"Error: circular buffer == NULL");
        exit(1);
    }
    // buffer_end punterà all'ultimo indirizzo di memoria dell'array
    fi->buffer_end = fi->buffer + capacity * sz;
    fi->capacity = capacity;
    fi->cursor = 0;
    fi->rcoursor = 0;
    fi->sz = sz;
    fi->head = fi->buffer;
    fi->tail = fi->buffer;
    return fi;
}

void fifo_free(circular_buffer *cb) {
    free(cb->buffer);
}

/**
 *
 * @param item
 * @param fi
 * @return
 */
int fifo_pop(void *item, circular_buffer *fi) {
    if (fi->cursor == 0) {
        fi->error_code=10;
        return 0;
    }
    memcpy(item, fi->tail, fi->sz);

    fi->tail = fi->tail + fi->sz;

    if (fi->tail == fi->buffer_end)
        fi->tail = fi->buffer;

    fi->cursor--;
    return 1;
}

/**
 * Partendo dall'ultimo elemento inserito cominci a leggere fino al primo
 * @param item
 * @param fi
 * @return
 */
size_t fifo_next(void *item, circular_buffer *fi) {
    if (fi->rcoursor == 0) {
        fi->error_code=10;
        return 0;
    }

    //procedura di ritorno del valore
    memcpy(item, fi->rtail, fi->sz);

    //procedure di aggironamento del puntatore
    fi->rtail = fi->rtail + fi->sz;

    if (fi->rtail == fi->buffer_end)
        fi->rtail = fi->buffer;

    fi->rcoursor--;
    return 1;
}

int fifo_errorcode(circular_buffer *cb){
    return cb->error_code;
}

/**
 * This function will delete the first element of the circular list
 * @param item
 * @param fi
 * @return
 */
int fifo_push(const void *item, circular_buffer *fi){
    //memcopy(destination, source, size)
    memcpy(fi->head, item, fi->sz);

    //Se il cursore ha raggiunto la massima capacità non incrementare
    if (fi->cursor == fi->capacity) {
        //Aggiorno il pointer a tail e lo posiziono al prossimo elemento
        fi->tail = fi->tail+fi->sz;
        if (fi->tail == fi->buffer_end)
            fi->tail = fi->buffer;
    }
    else{
        fi->cursor++;
    }

    //Aggiorno il pointer a head e lo posiziono al prossimo elemento
    fi->head = fi->head + fi->sz;

    //Se ho raggiunto la fine dell'array di supporto riporto la head alla base
    if (fi->head == fi->buffer_end)
        fi->head = fi->buffer;

    fi->rtail = fi->tail;
    fi->rcoursor = fi->cursor;
    return 1;
}