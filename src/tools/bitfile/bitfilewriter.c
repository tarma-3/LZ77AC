//
// Created by Enrico on 16.12.17.
//

#include "bitfilewriter.h"
//
// Created by Enrico on 16.12.17.
//

#ifndef LZ77AC_BITFILE_H
#define LZ77AC_BITFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if DEBUG_BFW_LOG
    #define LOG_DIR "./log"
FILE *log_bfw_output;
#endif

typedef struct _bitfile {
    FILE *_file;
    unsigned char bitbuffer;
    unsigned short bbsize;
} BitfileWriter;


BitfileWriter *bfopen(const char *filename) {
#if DEBUG_BFW_LOG
    log_bfw_output = fopen(LOG_DIR "/log_bitfilewriter-binoutput.txt", "wb+");
    fclose(log_bfw_output);
#endif
    BitfileWriter *bf = malloc(sizeof(BitfileWriter));
    bf->_file = fopen(filename, "w");
    //fclose(bf->_file);
    bf->bbsize = bf->bitbuffer = 0;
    return bf;
}

int bfclose(BitfileWriter *fp) {
    int r = fclose(fp->_file);
    free(fp);
    return r;
}

void _writechar(BitfileWriter *fp){
    int towrite = fp->bitbuffer;
    fputc(towrite,fp->_file);
#if DEBUG_BFW_LOG
    unsigned char binaryrap[9];
    log_bfw_output = fopen(LOG_DIR "/log_bitfilewriter-binoutput.txt", "a");
    for (int i = 0; i < 8; ++i) {
        binaryrap[i] = '0' + (((fp->bitbuffer) >> (7 - i)) & 1);
    }
    binaryrap[8]='\0';
    fprintf(log_bfw_output,"%s ",binaryrap);
    fclose(log_bfw_output);
#endif
}

void write_flush(BitfileWriter *fp) {
    fprintf(fp->_file, "%c", fp->bitbuffer);
    fp->bbsize = 0;
}

void write_bit(unsigned int bit, BitfileWriter *fp) {
    bit &= 1;
    bit<<=7-fp->bbsize;
    fp->bitbuffer |= bit;
    if (++fp->bbsize == 8) {

        _writechar(fp);

        fp->bbsize = 0;
        fp->bitbuffer = 0;
    }
}

void write_bits(unsigned int nofbitstow, unsigned int bitstow, BitfileWriter *fp) {
    unsigned int mask = (unsigned int) ((1 << nofbitstow) - 1);
    bitstow &= mask;
    unsigned int bk_bitstow = bitstow;
    short freebits = (short) 8 - fp->bbsize;

    if (nofbitstow >= freebits) {
        //DEVO SCRIVERE
        bitstow >>= nofbitstow - freebits;
        fp->bitbuffer |= bitstow;

        _writechar(fp);

        fp->bbsize = fp->bitbuffer = 0;
        if (nofbitstow > freebits){
            //Se ho ancora dei byte da scrivere
            write_bits(nofbitstow - freebits, bk_bitstow, fp);
        }
    } else {
        bitstow <<= abs(freebits - nofbitstow);
        fp->bitbuffer |= bitstow;
        fp->bbsize =  8-(freebits - nofbitstow);
    }
}




#endif //LZ77AC_BITFILE_H
