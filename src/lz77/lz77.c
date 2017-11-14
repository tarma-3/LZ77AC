//
// Created by Enrico on 09/10/2017.
//

#include <sys/stat.h>
#include "lz77.h"
#include "../tools/charcb/charcb.h"
#include "../tools/file/file.h"
#include "../tools/kmp/kmpsearch.h"

#define DEFAULT_OFFSET 2048
#define DICTIONARY_SIZE 32
#define LOOKAHEADB_SIZE 20

#if DEBUG_LZ77_LOG
#define CHECK_COPY "log_lz77-copy-check"
FILE *log_lz77;
FILE *log_lz77_vpassages;
FILE *log_lz77_settings;
size_t extlen;
#endif

//Numero caratteri massimi nel dizionario
int offset = DEFAULT_OFFSET;

CircularBuffer *dictionary;
char lookahead_buffer[LOOKAHEADB_SIZE];
// 1123451231231231

void init_encode(char source[], char outdir[]) {
    dictionary = cb_init(DICTIONARY_SIZE, sizeof(unsigned char));
    lookahead_buffer[0]='\0';

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
    log_lz77_vpassages = fopen("./log/log_lz77-vpassages.txt", "wb+");
#endif

}

void run_encode(unsigned char a) {
    static size_t pattern_index = 0;
    static long matchpos = -1;
    static long pmatchpos = -1;
    lookahead_buffer[pattern_index]=a;
    lookahead_buffer[pattern_index+1]='\0';
    pmatchpos = matchpos;
    // Se avevo trovato qualcosa nel carattere precedente
    if (pattern_index&&uccb_hasnext(dictionary)&&uccb_next(dictionary)==lookahead_buffer[pattern_index]){
#if DEBUG_LZ77_LOG
        fprintf(log_lz77_vpassages,"   +   found %2ld, pattern [%s]\n",matchpos,lookahead_buffer);
#endif
        pattern_index++;
    }
    else if((matchpos=structmatchn(lookahead_buffer,dictionary,pattern_index))==-1){
        // CASO IN CUI NON TROVO NIENTE
#if DEBUG_LZ77_LOG
        fprintf(log_lz77_vpassages,"Definitive   %2ld, pattern [%s]\n",pmatchpos,lookahead_buffer);
#endif
        uccb_reset(dictionary);
        pattern_index = 0;
        matchpos = -1;
        pmatchpos = -1;
    } else {
        // CASO IN CUI TROVO QUALCOSA
#if DEBUG_LZ77_LOG
        if(pmatchpos!=-1){
            fprintf(log_lz77_vpassages,"! Match interrupted, better option found\n");
        }
#endif
        pattern_index++;
#if DEBUG_LZ77_LOG
        fprintf(log_lz77_vpassages," Match found %2ld, pattern [%s]\n",matchpos,lookahead_buffer);
#endif
    }
    uccb_push(a, dictionary);
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