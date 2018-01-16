//
// Created by Enrico on 26.12.17.
//

#include <stdio.h>
#include <stdlib.h>
#include "bitfilereader.h"

typedef struct _bitfiler {
    FILE *file;
    unsigned char bitbuffer;
    unsigned short bbsize;
    unsigned int eof;
} BitfileReader;

BitfileReader *newBitfileReader(const char *path) {
    BitfileReader *bf = malloc(sizeof(BitfileReader));
    bf->bbsize = bf->bitbuffer = 0;
    bf->file = fopen(path,"r");
    bf->eof  = 0;
    return bf;
}

unsigned char read_bit(BitfileReader *fp) {
    if (fp->bbsize == 0) {
        int newbyte;
        if ((newbyte = fgetc(fp->file)) == EOF) {

        }
    }
}

unsigned int read_bits(unsigned int bitsreq, BitfileReader *fp) {
    if(fp->eof){
        fprintf(stderr, "BitfileReader: Error on reading\n");
        exit(101);
    }
    unsigned int tmp = 0;
    unsigned int rtn = 0;
    static int missingbits = 0;
    missingbits = bitsreq - fp->bbsize;
    // Ho bit a sufficenza?
    while (missingbits > 0) {
        // Se no devo caricarli dal _file
        tmp = fp->bitbuffer;
        //Gli scifto a sinistra lasciando spazio per i mancanti
        tmp <<= missingbits;
        rtn |= tmp;

        int newbyte = fgetc(fp->file);
        //Ricavo le nuove informazioni
        if(newbyte == EOF){
            fp->eof = 1;
            return rtn;
        }
        //Le carico in bitbuffer
        fp->bitbuffer = newbyte;
        //Aggiorno la grandezza di bitbuffer
        fp->bbsize = 8;
        missingbits = missingbits - fp->bbsize;
    }
    tmp = fp->bitbuffer;
    tmp >>= abs(missingbits);
    rtn |= tmp;
    fp->bitbuffer &= (1 << abs(missingbits)) - 1;
    fp->bbsize = (unsigned char) abs(missingbits);

    return rtn;
}

unsigned int read_checkEOF(BitfileReader *fp) {
    return fp->eof;
}