//
// Created by Enrico on 12.11.17.
//

#include "charcb.h"

// "Constructors & destructors"
CircularBuffer *uccb_init(size_t capacity){
    return cb_init(sizeof(unsigned char),capacity);
}
void uccb_free(CircularBuffer *cb){
    return cb_free(cb);
};

// Setters
bool uccb_push(unsigned char uchar,CircularBuffer *cb){
    return cb_push(&uchar,cb);
}
size_t uccb_getid(CircularBuffer *cb){
    return cb_getid(cb);
}

unsigned char uccb_read(size_t id, CircularBuffer *cb){
    unsigned char t;
    cb_read(id,&t,cb);
    return t;
}
unsigned char uccb_next(CircularBuffer *cb){
    unsigned char t;
    cb_next(&t,cb);
    return t;
}
unsigned char uccb_pointed(CircularBuffer *cb){
    unsigned char t;
    cb_pointed(&t,cb);
    return t;
}

bool uccb_hasnext(CircularBuffer *cb){
    return cb_hasnext(cb);
};

void uccb_reset(CircularBuffer *cb){
    cb_reset(cb);
}

size_t uccb_nofchars(CircularBuffer *cb){
    return cb_nofel(cb);
}

/*int uccb_errorcode(CircularBuffer *cb){
    return cb_errorcode(cb);
};*/