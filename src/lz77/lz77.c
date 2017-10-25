//
// Created by Enrico on 09/10/2017.
//

#include <sys/stat.h>
#include "lz77.h"
#include "../tools/fifo.h"
#include "../tools/file.h"

#define DEFAULT_OFFSET 2048
#define DICTIONARY_SIZE 10

#if DEBUG_LZ77_LOG
#define CHECK_COPY "log_lz77-copy-check"

FILE *log_lz77;
FILE *log_lz77_settings;
size_t extlen;

#endif

//Numero caratteri massimi nel dizionario
int offset = DEFAULT_OFFSET;

struct fifo *dictionary;
// 1123451231231231

void init_encode(char source[], char outdir[]) {
    dictionary = fifo_init(DICTIONARY_SIZE, sizeof(unsigned char));

#if DEBUG_LZ77_LOG
    extlen = get_extlen(source);
    size_t namesize = strlen(outdir) + 1 + strlen(CHECK_COPY) + extlen + 1;
    char log_copycheck[namesize];
    log_copycheck[0] = '\0';
    strcat(log_copycheck, outdir);
    strcat(log_copycheck, "/");
    strcat(log_copycheck, CHECK_COPY);
    strcat(log_copycheck, get_filename_ext(source));
    log_lz77 = fopen(log_copycheck, "wb+");
#endif
}

void run_encode(unsigned char a) {
    unsigned char i;
    while (fifo_hasnext(dictionary)){
        size_t pos = fifo_next(&i,dictionary);
        if(i == a){
            printf("Trovato %c alla posizione %zu\n",a,pos);
            break;
        }
    }
    fifo_push(&a, dictionary);

#if DEBUG_LZ77_LOG
    fprintf(log_lz77, "%c", a);
#endif
}

char *lz77_encode(char args[]) {
#if DEBUG_LZ77_LOG
    mkdir("./log", 0777);
    log_lz77_settings = fopen("./log/log_lz77-settings.txt", "wb+");
    fprintf(log_lz77_settings, "LZ77 SETTINGS\n"
            "OFFSET:\t\t\t\t%d\n"
            "DICTIONARY SIZE: \t%d\n"
            "FILE EXTENSION:\t\t%s\n", DEFAULT_OFFSET, DICTIONARY_SIZE, get_filename_ext(args));
#endif
    init_encode(args, "./log");
    stream_file_to(args, run_encode);
    return "f";
}