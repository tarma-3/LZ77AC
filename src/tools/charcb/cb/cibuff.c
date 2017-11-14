//
// Created by Enrico on 06.12.16.
//

#include "cibuff.h"

typedef struct _circularbuffer {
    void *buffer;       // pointer to buffer
    size_t capacity;    // maximum number of items in the buffer [1;n]
    size_t elements;    // number of items in the buffer [0;n]
    size_t fi_index;    // First-In array index [0;(n-1)]
    size_t cu_index;    // Cursor array index [0;(n-1)]
    size_t sz;          // size of each item in the buffer
    int error_code;
    bool hasNext;
} CircularBuffer;

#if DEBUG_FIFO_VERBOSE_LOG
FILE *log_fifoInsert;
void log_insertion(const void *item, CircularBuffer *cb){
    unsigned char log_m;
    memcpy(&log_m, item, cb->sz);
    fprintf(log_fifoInsert,"\n[+ new item] %c | Cursor id: %zu | First In Id: %zu\n",log_m,cb->cu_index,cb->fi_index);

    for(int i=0;i<cb->capacity;i++){
        if(cb->fi_index == i)
            fprintf(log_fifoInsert," %2s", "FI");
        else if(cb->cu_index == i)
            fprintf(log_fifoInsert," %2s", "CU");
        else fprintf(log_fifoInsert," %02d",i);
    }
    fprintf(log_fifoInsert,"\n");

    for(int i=0;i<cb->capacity;i++){
        memcpy(&log_m,&cb->buffer[i],cb->sz);
        fprintf(log_fifoInsert,"  %c",log_m);
    }
    fprintf(log_fifoInsert,"\n");
}
#endif

CircularBuffer *cb_init(size_t capacity, size_t sz) {

#if DEBUG_FIFO_VERBOSE_LOG
    mkdir("./log",0777);
    log_fifoInsert = fopen("./log/log_verb-CircularBuffer.txt", "wb+");
#endif

    if(capacity <= 0){
        fprintf(stderr,"Error: CircularBuffer must have at least 1 element");
        exit(1);
    }
    CircularBuffer *fi;
    fi = malloc(sizeof(struct _circularbuffer));
    // Alloco la memoria necessaria per contenere l'array di elementi
    fi->buffer = malloc(capacity * sz);
    if (fi->buffer == NULL) {
        fprintf(stderr,"Error: circular buffer == NULL");
        exit(1);
    }
    fi->capacity = capacity;
    fi->elements = 0;
    fi->fi_index = 0;
    fi->cu_index = 0;
    fi->sz = sz;
    fi->hasNext = false;

#if DEBUG_FIFO_VERBOSE_LOG
    fprintf(log_fifoInsert,"FIFO INIT: [capacity] %zu\n-\n",fi->capacity);
#endif

    return fi;
}

void cb_reset(CircularBuffer *cb){
    cb->cu_index = cb->fi_index;
    if(cb->elements>1){
        cb->hasNext = true;
    }
    else cb->hasNext = false;
}

void cb_free(CircularBuffer *cb) {
    free(cb->buffer);
}


/**
 * Partendo dall'ultimo elemento inserito cominci a leggere fino al primo
 * @param item
 * @param cb
 * @return
 */


size_t cb_getid(CircularBuffer *cb){
    long distance = cb->fi_index-cb->cu_index;
    if (distance <=0)distance +=cb->elements;
    return (size_t)(cb->elements-distance);
}



/**
 * This function
 * @param item
 * @param cb
 * @return
 */
bool cb_push(const void *item, CircularBuffer *cb){
    cb->hasNext = true;
    // Nel caso il buffer non sia pieno e quindi non sovrascriviamo nessun carattere
    if(cb->elements<cb->capacity){
        //printf("Inserisco in cb->buffer[%zu]\n ",cb->elements);
        memcpy(&(cb->buffer[cb->elements]), item, cb->sz);
        cb->elements++;

#if DEBUG_FIFO_VERBOSE_LOG
        log_insertion(item,cb);
#endif

        return 0;
    }
    // Nel caso il buffer sia pieno e quindi sovrascriviamo il FI
    memcpy(&(cb->buffer[cb->fi_index]), item, cb->sz);
    if (cb->fi_index==cb->cu_index)
        cb->cu_index=++cb->cu_index%cb->capacity;
    (++(cb->fi_index));
    cb->fi_index = cb->fi_index%cb->capacity;

#if DEBUG_FIFO_VERBOSE_LOG
    log_insertion(item,cb);
#endif

    return 1;
}

bool cb_hasnext(CircularBuffer *fi) {
    return fi->hasNext;
}


void _updatenext(CircularBuffer *cb){
    cb->cu_index= (++cb->cu_index)%cb->elements;
    if((cb->cu_index)%cb->elements==cb->fi_index) cb->hasNext = false;
}
/**
 *
 * @param item The item to push
 * @param cb Pointer to struct
 * @return
 */
size_t cb_next(void *item, CircularBuffer *cb) {

    if(!cb_hasnext(cb)){
        fprintf(stderr,"Error: FIFO -> End of buffer\n");
        exit(1);
    }
    memcpy(item,&(cb->buffer[cb->cu_index]), cb->sz);

    size_t elementID = cb_getid(cb);

    _updatenext(cb);

#if DEBUG_FIFO_VERBOSE_LOG
    unsigned char log_m;
    memcpy(&log_m, item, cb->sz);
    fprintf(log_fifoInsert,"[= next item] %c | element_id %2zu | next cu_index: %2zu | "
            "Has next: ",log_m,elementID,cb->cu_index);
    fprintf(log_fifoInsert,cb->hasNext ? "true\n" : "false\n");
#endif
    return elementID;
}

size_t cb_pointed(void *item, CircularBuffer *cb) {

    memcpy(item,&(cb->buffer[cb->cu_index]), cb->sz);

    size_t elementID = cb_getid(cb);

#if DEBUG_FIFO_VERBOSE_LOG
    unsigned char log_m;
    memcpy(&log_m, item, cb->sz);
    fprintf(log_fifoInsert,"[= poin item] %c | element_id %2zu | next cu_index: %2zu | "
            "Has next: ",log_m,elementID,cb->cu_index);
    fprintf(log_fifoInsert,cb->hasNext ? "true\n" : "false\n");
#endif
    return elementID;
}

size_t cb_nofel(CircularBuffer *cb){
    return cb->elements;
}

void cb_read(size_t id, void *item, CircularBuffer *cb) {
    if(id >= cb->capacity){
        fprintf(stderr,"Error: Circular Buffer -> Out of bounds\n");
        exit(1);
    }
    memcpy(item,&(cb->buffer[(cb->fi_index+id)%cb->elements]), cb->sz);
    cb->cu_index = (cb->fi_index+id)%cb->elements;

    _updatenext(cb);

#if DEBUG_FIFO_VERBOSE_LOG
    unsigned char log_m;
    memcpy(&log_m, item, cb->sz);
    fprintf(log_fifoInsert,"[= read item] %c | element_id %2zu | next cu_index: %2zu | "
            "Has next: ",log_m,id,cb->cu_index);
    fprintf(log_fifoInsert,cb->hasNext ? "true\n" : "false\n");
#endif

}
