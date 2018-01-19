

#include <stdlib.h>
#include "../../tools/bytestr/bytestr.h"
#include "../../tools/bitfile/bitfilereader.h"
#include "../lz77.h"
#include "../../tools/scharcb/scharcb.h"

const unsigned int DICTIONARY_SIZE;
const unsigned int LOOKAHEADB_SIZE;

static BitfileReader *bfreader;
static FILE *foutput;

//SCOPO DI DEBUG
static long charswritten = 0;

typedef struct _tern {
    long flag;
    long dictpos;
    long len;
    unsigned char c;
} Tern;

int debugatchar = 0;

void recovercharfromdict(size_t dictsize, Tern tern);

typedef enum _decodestatus {
    DECODE_FLAG, DECODE_DICTPOS, DECODE_LEN, DECODE_CHAR, DECODE_DCHAR
} DecodeStatus;

void initdecompressor(char *source, char *destination) {
    DEBUG_ENABLED = 0;
    bfreader = newBitfileReader("./output.press");
    foutput = fopen("./output.txt", "w");
#if DEBUG_LZ77_LOG
    __init_dec_log(source);
#endif
}

void rundecompression() {
    static DecodeStatus decodeStatus = DECODE_FLAG;
    static Tern tern;
    int isEOF = 0;
    while (isEOF == 0) {
        //TODO: DEBUG ONLY
        if (charswritten >= debugatchar)DEBUG_ENABLED = 1;
        //if(charswritten>=debugatchar+20)exit(0);
        switch (decodeStatus) {
            case DECODE_FLAG:
                tern.flag = read_bits(1, bfreader);
                isEOF = read_checkEOF(bfreader);
                if (isEOF) {
                    break;
                }
                if (tern.flag) {
                    decodeStatus = DECODE_DICTPOS;
#if DEBUG_LZ77_LOG
                    if (DEBUG_ENABLED)__log_decode_FLAG1_FOUND();
#endif
                } else {
                    decodeStatus = DECODE_DCHAR;
#if DEBUG_LZ77_LOG
                    if (DEBUG_ENABLED)__log_decode_FLAG0_FOUND();
#endif
                }
                break;
            case DECODE_DICTPOS:
                tern.dictpos = read_bits(11, bfreader);
                isEOF = read_checkEOF(bfreader);
                if (isEOF) {
                    break;
                }
                decodeStatus = DECODE_LEN;
#if DEBUG_LZ77_LOG
                if (DEBUG_ENABLED) __log_decode_DICTPOS(tern.dictpos);
#endif
                break;
            case DECODE_LEN:
                tern.len = read_bits(4, bfreader);
                isEOF = read_checkEOF(bfreader);
                if (isEOF) {
                    break;
                }
                decodeStatus = DECODE_CHAR;
#if DEBUG_LZ77_LOG
                if (DEBUG_ENABLED)__log_decode_LEN(tern.len);
#endif
                break;
            case DECODE_CHAR:
                tern.c = (unsigned char) read_bits(8, bfreader);
                isEOF = read_checkEOF(bfreader);
                if (isEOF) {
                    recovercharfromdict(uccb_nofchars(), tern);
                    break;
                }
                recovercharfromdict(uccb_nofchars(), tern);
                //Print new char
                fprintf(foutput, "%c", tern.c);
                uccb_push(tern.c);
                decodeStatus = DECODE_FLAG;
                isEOF = read_checkEOF(bfreader);
#if DEBUG_LZ77_LOG
                if (DEBUG_ENABLED) __log_decode_NEW_CHAR(tern.c);
#endif
                charswritten++;
                if (charswritten >= debugatchar)DEBUG_ENABLED = 1;
                break;
            case DECODE_DCHAR:
                tern.c = (unsigned char) read_bits(8, bfreader);
                isEOF = read_checkEOF(bfreader);
                if (isEOF) {
                    break;
                }
                fprintf(foutput, "%c", tern.c);
                uccb_push(tern.c);

                decodeStatus = DECODE_FLAG;
                isEOF = read_checkEOF(bfreader);
#if DEBUG_LZ77_LOG
                if (DEBUG_ENABLED) __log_decode_NEW_DCHAR(tern.c);
#endif
                charswritten++;
                if (charswritten >= debugatchar)DEBUG_ENABLED = 1;
                break;
            default:
                fprintf(stderr, "_run_decode exception");
                exit(23);
        }
    }
    //fclose(foutput);
    //closeBitfileReader(bfreader);
}

void recovercharfromdict(size_t dictsize, Tern tern) {

    // Mi posiziono tanti passi indietro quanti mi è stato detto da dictpos
    uccb_setid(dictsize - (unsigned int) tern.dictpos);
    //printf("%d ",tern.dictpos);
    //Attenzione, se mi posiziono in First In, il next char viene fatto automaticamente con i push
    //In questo momento sto puntando al primo carattere in cb
#if DEBUG_LZ77_LOG
    if (DEBUG_ENABLED) __log_decode_PRINT_DICTIONARY_REC();
#endif
    for (int i = 0; i < tern.len; ++i) {
        unsigned char chrec;
        if (uccb_hasnext())
            chrec = uccb_next();
        else {
            fprintf(stderr, "Error on recovercharfromdict\n");
            exit(0);
        }
        uccb_push(chrec);
        fprintf(foutput, "%c", chrec);
#if DEBUG_LZ77_LOG
        if (DEBUG_ENABLED) __log_decode_PRINT_RECOVERED_CHARS(chrec);
#endif
        charswritten++;
        if (charswritten >= debugatchar)DEBUG_ENABLED = 1;
    }
}