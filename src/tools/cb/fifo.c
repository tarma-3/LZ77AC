//
// Created by Enrico on 06.12.16.
//

#include "fifo.h"

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
        fprintf(log_fifoInsert," %02d",i);
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



void cb_free(CircularBuffer *cb) {
    free(cb->buffer);
}


/**
 * Partendo dall'ultimo elemento inserito cominci a leggere fino al primo
 * @param item
 * @param cb
 * @return
 */


size_t cb_setid(size_t id, CircularBuffer *cb){
    //TODO: Mettere a posto
    if(0 >= id || id > cb->elements){
        fprintf(stderr,"Errore: valore cursore non valido, range da [1 - %zu]\n"
                " Input: %zu\n",cb->elements,id);
        exit(1);
    }
   // cb->cu_index = ctoi(id, cb);
    return 0;
}

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
int cb_push(const void *item, CircularBuffer *cb){
    cb->hasNext = true;
    if(cb->elements<cb->capacity){
        //printf("Inserisco in cb->buffer[%zu]\n ",cb->elements);
        memcpy(&(cb->buffer[cb->elements]), item, cb->sz);
        cb->elements++;
        cb->cu_index = 0;

        #if DEBUG_FIFO_VERBOSE_LOG
        log_insertion(item,cb);
        #endif

        return 0;
    }
    //printf("Inserisco in cb->buffer[%zu]\n ",cb->fi_index);
    memcpy(&(cb->buffer[cb->fi_index]), item, cb->sz);
    (++(cb->fi_index));
    cb->fi_index = cb->fi_index%cb->capacity;
    cb->cu_index = cb->fi_index;

    #if DEBUG_FIFO_VERBOSE_LOG
    log_insertion(item,cb);
    #endif

    return 1;
}

size_t fifo_read(void *item, size_t id, CircularBuffer *cb) {
    cb_setid(id,cb);
    memcpy(item,&(cb->buffer[cb->cu_index]), cb->sz);
    if((cb->cu_index+1)%cb->elements==cb->fi_index) cb->hasNext = false;
    return 1;
}

bool cb_hasnext(CircularBuffer *fi) {
    return fi->hasNext;
}

/**
 *
 * @param item The item to push
 * @param fi Pointer to struct
 * @return
 */
size_t cb_next(void *item, CircularBuffer *fi) {

    if(!cb_hasnext(fi)){
        fprintf(stderr,"Error: FIFO -> End of buffer\n");
        exit(1);
    }
    memcpy(item,&(fi->buffer[fi->cu_index]), fi->sz);

    size_t elementID = cb_getid(fi);

    fi->cu_index= (++fi->cu_index)%fi->elements;
    if((fi->cu_index)%fi->elements==fi->fi_index) fi->hasNext = false;

#if DEBUG_FIFO_VERBOSE_LOG
    unsigned char log_m;
    memcpy(&log_m, item, fi->sz);
    fprintf(log_fifoInsert,"[+ next item] %c | element_id %2zu | next cu_index: %02zu | "
            "Has next: ",log_m,elementID,fi->cu_index);
    fprintf(log_fifoInsert,fi->hasNext ? "true\n" : "false\n");
#endif
    return elementID;
}

