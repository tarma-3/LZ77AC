//
// Created by Enrico on 16.12.17.
//

#include "bitfilewriter.h"
#include "../utility.h"
#include "../../lz77/lz77.h"
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
#define BITFILE_STEPS "log_bitfilewriter-steps.txt"
FILE *log_bfw_output;
FILE *log_bfw_steps;
#endif

typedef struct _bitfile {
    FILE *_file;
    unsigned char bitbuffer;
    unsigned short bbsize;
} BitfileWriter;


BitfileWriter *newBitFileWriter(const char *filename) {
#if DEBUG_BFW_LOG
    log_bfw_output = fopen(LOG_DIR "/log_bitfilewriter-binoutput.txt", "w");
    fclose(log_bfw_output);
    log_bfw_steps = fopen(LOG_DIR "/" BITFILE_STEPS, "w");;
    fclose(log_bfw_steps);
#endif
    BitfileWriter *bf = malloc(sizeof(BitfileWriter));
    bf->_file = fopen(filename, "w");
    //fclose(bf->_file);
    bf->bbsize = bf->bitbuffer = 0;
    return bf;
}


void _writechar(BitfileWriter *fp) {
#if DEBUG_BFW_LOG
    if (DEBUG_ENABLED) log_bfw_steps = fopen(LOG_DIR "/" BITFILE_STEPS, "a");;
#endif
    fputc(fp->bitbuffer, fp->_file);
#if DEBUG_BFW_LOG
    if (DEBUG_ENABLED) {
        static int i = 0;
        i++;
        log_bfw_output = fopen(LOG_DIR "/log_bitfilewriter-binoutput.txt", "a");
        fprintf(log_bfw_output, "%s ", __tobinary(8, fp->bitbuffer));
        fprintf(log_bfw_steps, "-\nCALLED write_byte(%s)\n-\n", __tobinary(8, fp->bitbuffer));
        if (i == 13) {
            fputc('\n', log_bfw_output);
            i = 0;
        }
        fclose(log_bfw_output);
        fclose(log_bfw_steps);
    }
#endif
}

int flushCloseBitfileWriter(BitfileWriter *fp) {
    _writechar(fp);
    int r = fclose(fp->_file);
    free(fp);
    return r;
}

void write_bit(unsigned int bit, BitfileWriter *fp) {
#if DEBUG_BFW_LOG
    char t1[9], t2[9];
    if (DEBUG_ENABLED) {
        log_bfw_steps = fopen(LOG_DIR "/" BITFILE_STEPS, "a");;
        fprintf(log_bfw_steps, "CALLED write_bit(%d)\n", bit);
        fprintf(log_bfw_steps, "bitbuffer before %s | [%8s] | val %d\n", _tobinary(t1, 8, fp->bitbuffer),
                _tobinarylr(t2, fp->bbsize, fp->bitbuffer), fp->bitbuffer);
        fclose(log_bfw_steps);
    }
#endif


    bit &= 1;
    bit <<= 7 - fp->bbsize;
    fp->bitbuffer |= bit;
    if (++fp->bbsize == 8) {
        _writechar(fp);
        fp->bbsize = 0;
        fp->bitbuffer = 0;
    }

#if DEBUG_BFW_LOG
    if (DEBUG_ENABLED) {
        log_bfw_steps = fopen(LOG_DIR "/" BITFILE_STEPS, "a");;
        fprintf(log_bfw_steps, "bitbuffer after  %s | [%8s] | val %d\n\n", _tobinary(t1, 8, fp->bitbuffer),
                _tobinarylr(t2, fp->bbsize, fp->bitbuffer), fp->bitbuffer);
        fclose(log_bfw_steps);
    }
#endif
}

void write_bits(unsigned int nofbitstow, unsigned int bitstow, BitfileWriter *fp) {
#if DEBUG_BFW_LOG
    char t1[9], t2[9];
    if (DEBUG_ENABLED) {
        log_bfw_steps = fopen(LOG_DIR "/" BITFILE_STEPS, "a");;
        fprintf(log_bfw_steps, "CALLED write_bits(%s)\n", __tobinary(nofbitstow, bitstow));
        fprintf(log_bfw_steps, "bitbuffer before %s | [%8s] | val %d\n", _tobinary(t1, 8, fp->bitbuffer),
                _tobinarylr(t2, fp->bbsize, fp->bitbuffer), fp->bitbuffer);
        fclose(log_bfw_steps);
    }
#endif

    unsigned int mask = (unsigned int) ((1 << nofbitstow) - 1);
    bitstow &= mask;
    unsigned int bk_bitstow = bitstow;
    short freebits = (short) 8 - fp->bbsize;

    if (nofbitstow >= freebits) {
        // Il bitbuffer è pieno, devo liberarlo stampando
        bitstow >>= nofbitstow - freebits;
        fp->bitbuffer |= bitstow;
        _writechar(fp);
        fp->bbsize = fp->bitbuffer = 0;
        if (nofbitstow > freebits) {
            //Se ho ancora dei byte da scrivere
            write_bits(nofbitstow - freebits, bk_bitstow, fp);
        }
    } else {
        bitstow <<= abs(freebits - nofbitstow);
        fp->bitbuffer |= bitstow;
        fp->bbsize = (unsigned short) (8 - (freebits - nofbitstow));
    }

#if DEBUG_BFW_LOG
    if (DEBUG_ENABLED) {
        log_bfw_steps = fopen(LOG_DIR "/" BITFILE_STEPS, "a");;
        t1[9], t2[9];
        fprintf(log_bfw_steps, "bitbuffer after  %s | [%8s] | val %d\n\n", _tobinary(t1, 8, fp->bitbuffer),
                _tobinarylr(t2, fp->bbsize, fp->bitbuffer), fp->bitbuffer);
        fclose(log_bfw_steps);
    }
#endif
}


#endif //LZ77AC_BITFILE_H
