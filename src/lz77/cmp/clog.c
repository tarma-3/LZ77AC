//
// Created by Enrico on 28.12.17.
//

#include "../../tools/file/file.h"
#include "../../tools/bytestr/bytestr.h"
#include "../../tools/utility.h"
#include <sys/stat.h>
#include <string.h>

#define LOG_DIR "./log/cmp"

#define CMP_BINARY_ONLY_FILENAME "cmp_duringcmp_bitview.txt"
#define POST_COMPRESSION_BITVIEW_FILENAME "cmp_postcmp_bitview.txt"
#define SETTINGS_LOG_FILENAME "cmp_settings.txt"
#define CMP_DATA_PLUS_BINARY_FILENAME "cmp_data+binary.txt"
#define COPY_CHECK_FILENAME "cmp_copy-check"
#define STEPS_LOG_FILENAME "cmp_passages.txt"

const unsigned int DICTIONARY_SIZE;
static FILE *logfile_STEPS;
static FILE *logfile_binaryout;
static FILE *log_lz77_settings;
static FILE *log_lz77_writebin;
static FILE *post_compression_bitview;
static FILE *logfile_copycheck;


void __init_cmp_log(char *source) {
    // Creo la directory dove verranno salvati tutti i _file di log
    mkdir(LOG_DIR, 0777);

    // Creo un _file che si occupa di salvare le impostazioni del codec
    log_lz77_settings = fopen(LOG_DIR "/" SETTINGS_LOG_FILENAME, "wb+");
    {
        fprintf(log_lz77_settings,
                "LZ77 SETTINGS\n"
                        "SOURCE FILE:\t\t%s\n"
                        "DICTIONARY SIZE:\t%d\n"
                        "FILE EXTENSION:\t\t%s\n",
                source,
                DICTIONARY_SIZE,
                get_filename_ext(source));
    }
    fclose(log_lz77_settings);

    /* Creo una stringa per contenere il nome del _file di log
     * che si occupa del copy-check, ovvero si assicura che il
     * _file venga letto correttamente. Questo _file risultato
     * quindi lo stesso contenuto del _file sorgente.
     */
    char COPYCHECK_FILENAME[strlen(LOG_DIR) + 1 + strlen(COPY_CHECK_FILENAME) + get_extlen(source) + 1];
    {
        COPYCHECK_FILENAME[0] = '\0';
        strcat(COPYCHECK_FILENAME, LOG_DIR);
        strcat(COPYCHECK_FILENAME, "/");
        strcat(COPYCHECK_FILENAME, COPY_CHECK_FILENAME);
        strcat(COPYCHECK_FILENAME, get_filename_ext(source));
    }
    logfile_copycheck = fopen(COPYCHECK_FILENAME, "wb+");


    // Creo e apro il _file di log
    logfile_binaryout = fopen(LOG_DIR "/" CMP_DATA_PLUS_BINARY_FILENAME, "wb+");
    fclose(logfile_binaryout);

    log_lz77_writebin = fopen(LOG_DIR "/" CMP_BINARY_ONLY_FILENAME, "wb+");
    fclose(log_lz77_writebin);

    // Creo il file per i passaggi
    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "wb+");
    fputs("CALL _run_encode(char/numvalue)\n", logfile_STEPS);
    fputs("Match found [dict_id/offset]\n", logfile_STEPS);
    fputs(" --- \n", logfile_STEPS);
    fclose(logfile_STEPS);

    post_compression_bitview = fopen(LOG_DIR "/" POST_COMPRESSION_BITVIEW_FILENAME, "w");
}


static char binaryrap[25];

void __log_lz77_init_BINARY_RAP() {
    binaryrap[0] = '\0';
}

void __log_lz77_dcwrite_FLAG1(size_t offset, ByteString *lookahead_buffer) {
    logfile_binaryout = fopen(LOG_DIR "/" CMP_DATA_PLUS_BINARY_FILENAME, "a");
    fprintf(logfile_binaryout, "[1,%4zu,%4zu,", offset, bs_getlen(lookahead_buffer));
    fclose(logfile_binaryout);

    binaryrap[0] = '1';

    char *tmp = __tobinary(11, offset);
    strcpy(&binaryrap[1], tmp);
    tmp = __tobinary(4, bs_getlen(lookahead_buffer) - 1);
    strcpy(&binaryrap[12], tmp);
    tmp = __tobinary(8, bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1));
    strcpy(&binaryrap[16], tmp);
}

void __log_lz77_dcwrite_FLAG1_E(size_t offset, size_t pattern_len) {
    logfile_binaryout = fopen(LOG_DIR "/" CMP_DATA_PLUS_BINARY_FILENAME, "a");
    fprintf(logfile_binaryout, "[1,%4zu,%4zu,", offset, pattern_len);
    fclose(logfile_binaryout);

    binaryrap[0] = '1';
    char *tmp = __tobinary(11, offset);
    strcpy(&binaryrap[1], tmp);
    tmp = __tobinary(4, pattern_len);
    strcpy(&binaryrap[12], tmp);
}

void __log_lz77_dcwrite_FLAG0(ByteString *lookahead_buffer) {
    logfile_binaryout = fopen(LOG_DIR "/" CMP_DATA_PLUS_BINARY_FILENAME, "a");
    fprintf(logfile_binaryout, "[0, ");
    fclose(logfile_binaryout);
    binaryrap[0] = '0';
    char *tmp = __tobinary(8, bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1));
    strcpy(&binaryrap[1], tmp);
}

void __log_lz77_dcwrite_NEWCHAR(ByteString *lookahead_buffer) {
    unsigned char newchar = bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1);
    logfile_binaryout = fopen(LOG_DIR "/" CMP_DATA_PLUS_BINARY_FILENAME, "a");
    fprintf(logfile_binaryout, "%c] %s\n", newchar, binaryrap);
    fclose(logfile_binaryout);

    log_lz77_writebin = fopen(LOG_DIR "/" CMP_BINARY_ONLY_FILENAME, "a");
    fprintf(log_lz77_writebin, "%s", binaryrap);
    fclose(log_lz77_writebin);
}

void __log_lz77_dcwrite_TERMINATE() {
    logfile_binaryout = fopen(LOG_DIR "/" CMP_DATA_PLUS_BINARY_FILENAME, "a");
    for (int i = 0; i < 8; ++i) {
        binaryrap[16 + i] = 'X';
    }
    binaryrap[24] = '\0';
    fprintf(logfile_binaryout, "-] %s\n", binaryrap);
    fclose(logfile_binaryout);

    log_lz77_writebin = fopen(LOG_DIR "/" CMP_BINARY_ONLY_FILENAME, "a");
    fprintf(log_lz77_writebin, "%s", binaryrap);
    fclose(log_lz77_writebin);

    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "a");
    fprintf(logfile_STEPS, "[END]\n");
    fclose(logfile_STEPS);

    fclose(logfile_copycheck);
}

void __log_lz77_vpassages_run_encode_CALL(unsigned char next_byte) {
    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "a");
    fprintf(logfile_STEPS, "CALL _run_encode(%c/%3d)\n", next_byte, next_byte);
    fclose(logfile_STEPS);
}

void __log_lz77_vpassages_CONSECUTIVE_MATCH(long dictmatch_id, size_t offset, ByteString *lookahead_buffer) {
    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "a");
    char t[bs_capacity(lookahead_buffer)];
    fprintf(logfile_STEPS, "   +   found %2ld/%2ld, pattern [%s]\t(consecutive)\n", dictmatch_id, offset,
            bs_string(lookahead_buffer, t));
    fclose(logfile_STEPS);
}

void __log_lz77_vpassages_DEFINITIVE_MATCH(long dictmatch_id, size_t offset, ByteString *lookahead_buffer) {
    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "a");
    char t[bs_capacity(lookahead_buffer)];
    fprintf(logfile_STEPS, "DEFINITIVE   %2ld/%2ld, pattern [%s]\n\n",
            dictmatch_id, offset, bs_string(lookahead_buffer, t));
    fclose(logfile_STEPS);
}

void __log_lz77_vpassages_BETTER_MATCH(long dictmatch_id) {
    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "a");
    if (dictmatch_id != -1) {
        fprintf(logfile_STEPS, "   *   Old match discharged, better option found\n");
    }
    fclose(logfile_STEPS);
}

void __log_lz77_vpassages_MATCH_FOUND(long dictmatch_id, size_t offset, ByteString *lookahead_buffer) {
    logfile_STEPS = fopen(LOG_DIR "/" STEPS_LOG_FILENAME, "a");
    char t[bs_capacity(lookahead_buffer)];
    fprintf(logfile_STEPS, " Match found %2ld/%2ld, pattern [%s]\n", dictmatch_id, offset,
            bs_string(lookahead_buffer, t));
    fclose(logfile_STEPS);
}

void _log_COPYCHECK(unsigned char next_byte) {
    fputc(next_byte, logfile_copycheck);
}

void __log_bitopen(unsigned char newchar) {

    post_compression_bitview = fopen(LOG_DIR "/" POST_COMPRESSION_BITVIEW_FILENAME, "a");
    for (int i = 0; i < 8; ++i) {
        fputc('0' + ((newchar >> (7 - i)) & 1), post_compression_bitview);
    }
    fclose(post_compression_bitview);
}