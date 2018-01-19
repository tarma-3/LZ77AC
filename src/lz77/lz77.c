//
// Created by Enrico on 09/10/2017.
//

#include <time.h>
#include <string.h>
#include "lz77.h"

#include "../tools/file/file.h"


#include "cmp/enc.h"
#include "dec/dec.h"
#include "cmp/clog.h"


#define FLAG_SIZE 1

const unsigned int LOOKAHEADB_SIZE = 16;
int DEBUG_ENABLED = 0;
#define CHAR_SIZE 8

/* ______FUNZIONI DI COMPRESSIONE/DECOMPRESSIONE______
 * Le funzioni private oltre a non essere visibili nel
 * _file header sono nominate con un trattino basso
 * prima del nome.
 */
// __________FUNZIONI PRINCIPALI


char *lz77_encode(char source[]) {

    double total_timespent = 0;
    clock_t begin = clock();

    initcompressor(get_filename_ext(source),"./tmp.press");
    stream_file_to(source, runcompression);
    printf("Terminating");
    terminatecompression();

    clock_t end = clock();
    //if(DEBUG_ENABLED) stream_file_to("./output.press", __log_bitopen);
     total_timespent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("TOT - Time [s]: %14.3lf \n", total_timespent);

    return "f";
}

char *lz77_decode(char args[],char dest[]) {
    //TODO: Parametrizzare questa chiamata
    dest[strlen(dest)-get_extlen(dest)]='\0';
    initdecompressor(args, dest);
    rundecompression();
    return "f";
}