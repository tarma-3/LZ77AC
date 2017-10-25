//
// Created by Enrico on 06.12.16.
//

#include "fifo.h"

typedef struct fifo {
    void *buffer;       // pointer to buffer
    size_t capacity;    // maximum number of items in the buffer [1;n]
    size_t elements;    // number of items in the buffer [0;n]
    size_t fi_index;    // First-In array index [0;(n-1)]
    size_t cu_index;    // Cursor array index [0;(n-1)]
    size_t sz;          // size of each item in the buffer
    int error_code;
    bool hasNext;
};

#if DEBUG_FIFO_VERBOSE_LOG
FILE *log_fifoInsert;
void log_insertion(const void *item, fifo *fi){
    unsigned char log_m;
    memcpy(&log_m, item, fi->sz);
    fprintf(log_fifoInsert,"\n[+ new item] %c | Cursor id: %zu | First In Id: %zu\n",log_m,fi->cu_index,fi->fi_index);

    for(int i=0;i<fi->capacity;i++){
        fprintf(log_fifoInsert," %02d",i);
    }
    fprintf(log_fifoInsert,"\n");

    for(int i=0;i<fi->capacity;i++){
        memcpy(&log_m,&fi->buffer[i],fi->sz);
        fprintf(log_fifoInsert,"  %c",log_m);
    }
    fprintf(log_fifoInsert,"\n");
}
#endif

fifo *fifo_init(size_t capacity, size_t sz) {

    #if DEBUG_FIFO_VERBOSE_LOG
        mkdir("./log",0777);
        log_fifoInsert = fopen("./log/log_verb-fifo.txt", "wb+");
    #endif

    if(capacity <= 0){
        fprintf(stderr,"Error: fifo must have at least 1 element");
        exit(1);
    }
    fifo *fi;
    fi = malloc(sizeof(struct fifo));
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



void fifo_free(fifo *cb) {
    free(cb->buffer);
}


/**
 * Partendo dall'ultimo elemento inserito cominci a leggere fino al primo
 * @param item
 * @param fi
 * @return
 */


size_t fifo_setid(size_t id, fifo *fi){
    //TODO: Mettere a posto
    if(0 >= id || id > fi->elements){
        fprintf(stderr,"Errore: valore cursore non valido, range da [1 - %zu]\n"
                " Input: %zu\n",fi->elements,id);
        exit(1);
    }
   // fi->cu_index = ctoi(id, fi);
    return 0;
}
size_t mod (long a, size_t b)
{
    if(a==0&b==0)return 0;
    long ret = a % b;
    if(ret < 0)
        ret+=b;
    return (size_t)ret;
}
size_t fifo_getid(fifo *fi){
    long distance = fi->fi_index-fi->cu_index;
    if (distance <=0)distance +=fi->elements;
    return (size_t)distance;
}



/**
 * This function
 * @param item
 * @param fi
 * @return
 */
int fifo_push(const void *item, fifo *fi){
    fi->hasNext = true;
    if(fi->elements<fi->capacity){
        //printf("Inserisco in fi->buffer[%zu]\n ",fi->elements);
        memcpy(&(fi->buffer[fi->elements]), item, fi->sz);
        fi->elements++;
        fi->cu_index = 0;

        #if DEBUG_FIFO_VERBOSE_LOG
        log_insertion(item,fi);
        #endif

        return 0;
    }
    //printf("Inserisco in fi->buffer[%zu]\n ",fi->fi_index);
    memcpy(&(fi->buffer[fi->fi_index]), item, fi->sz);
    (++(fi->fi_index));
    fi->fi_index = fi->fi_index%fi->capacity;
    fi->cu_index = fi->fi_index;

    #if DEBUG_FIFO_VERBOSE_LOG
    log_insertion(item,fi);
    #endif

    return 1;
}

size_t fifo_read(void *item, size_t id, fifo *fi) {
    fifo_setid(id,fi);
    memcpy(item,&(fi->buffer[fi->cu_index]), fi->sz);
    if((fi->cu_index+1)%fi->elements==fi->fi_index) fi->hasNext = false;
    return 1;
}

bool fifo_hasnext(fifo *fi) {
    return fi->hasNext;
}

/**
 *
 * @param item The item to push
 * @param fi Pointer to struct
 * @return
 */
size_t fifo_next(void *item, fifo *fi) {

    if(!fifo_hasnext(fi)){
        fprintf(stderr,"Error: FIFO -> End of buffer\n");
        exit(1);
    }
    memcpy(item,&(fi->buffer[fi->cu_index]), fi->sz);

    size_t elementID = fifo_getid(fi);

    fi->cu_index= (++fi->cu_index)%fi->elements;
    if((fi->cu_index)%fi->elements==fi->fi_index) fi->hasNext = false;

#if DEBUG_FIFO_VERBOSE_LOG
    unsigned char log_m;
    memcpy(&log_m, item, fi->sz);
    fprintf(log_fifoInsert,"[+ next item] %c | element_id %2.zu | next cu_index: %02zu | "
            "Has next: ",log_m,elementID,fi->cu_index);
    fprintf(log_fifoInsert,fi->hasNext ? "true\n" : "false\n");
#endif
    return elementID;
}

