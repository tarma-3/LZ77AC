//
// Created by Enrico on 09/10/2017.
//

#include <time.h>
#include "lz77.h"

#include "../tools/file/file.h"


#include "cmp/enc.h"
#include "dec/dec.h"
#include "cmp/clog.h"
#include "../tools/kmp/kmp.h"

#define FLAG_SIZE 1
const unsigned int DICTIONARY_SIZE = 2047;
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
    //TODO: Parametrizzare questa chiamata
    double total_timespent = 0;
   // clock_t begin = clock();

    initcompressor(get_filename_ext(source),"./output.press");
    stream_file_to(source, runcompression);
    terminatecompression();

   // clock_t end = clock();
    //if(DEBUG_ENABLED) stream_file_to("./output.press", __log_bitopen);
   // total_timespent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("KMP/Table - Time [s]: %8.3lf \n", kmp_buildtable_time);
    printf("KMP - Time [s]: %14.3lf \n", kmp_match_time);
    printf("TOT - Time [s]: %14.3lf \n", cb_hasnext_time);
    printf("TOT - Time [s]: %14.3lf \n", total_timespent);

    return "f";
}

char *lz77_decode(char args[]) {
    //TODO: Parametrizzare questa chiamata
    initdecompressor("./output.press", "./output");
    rundecompression();
    return "f";
}