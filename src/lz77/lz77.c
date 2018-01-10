//
// Created by Enrico on 09/10/2017.
//

#include "lz77.h"

#include "../tools/file/file.h"


#include "cmp/enc.h"
#include "dec/dec.h"
#include "cmp/clog.h"

#define FLAG_SIZE 1
const unsigned int DICTIONARY_SIZE = 2000;
const unsigned int LOOKAHEADB_SIZE = 16;
int DEBUG_ENABLED = 0;
#define CHAR_SIZE 8

/* ______FUNZIONI DI COMPRESSIONE/DECOMPRESSIONE______
 * Le funzioni private oltre a non essere visibili nel
 * _file header sono nominate con un trattino basso
 * prima del nome.
 */
// __________FUNZIONI PRINCIPALI


char *lz77_encode(char args[]) {
    //TODO: Parametrizzare questa chiamata
    initcompressor(args, "./output.press");
    stream_file_to(args, runcompression);
    terminatecompression();
    if(DEBUG_ENABLED) stream_file_to("./output.press", __log_bitopen);
    return "f";
}

char *lz77_decode(char args[]) {
    //TODO: Parametrizzare questa chiamata
    initdecompressor("./output.press", "./output");
    rundecompression();
    return "f";
}