//
// Created by Enrico on 28.12.17.
//

#include "../../tools/file/file.h"
#include "../../tools/bytestr/bytestr.h"
#include "../../tools/utility.h"
#include <sys/stat.h>
#include <string.h>

#define LOG_DIR "./log/dec"

#define VPASSAGES_FILENAME "dec_passages.txt"
#define POST_DECOMPRESSION_BITVIEW_FILENAME "dec_bitviewer.txt"

static FILE *log_lz77_vpassages;
static FILE *decompress_bitview;

void __init_dec_log(char *source){
    // Creo la directory dove verranno salvati tutti i _file di log
    mkdir(LOG_DIR, 0777);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "w");
    fclose(decompress_bitview);

    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "w");
    fclose(log_lz77_vpassages);
}

void __log_decode_FLAG1_FOUND() {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fputs("FLAG 1 | ", log_lz77_vpassages);
    fclose(log_lz77_vpassages);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "a");
    fputs(" ", decompress_bitview);
    fputs("1", decompress_bitview);
    fclose(decompress_bitview);
}

void __log_decode_FLAG0_FOUND() {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fputs("FLAG 0 | ", log_lz77_vpassages);
    fclose(log_lz77_vpassages);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "a");
    fputs(" ", decompress_bitview);
    fputs("0", decompress_bitview);
    fclose(decompress_bitview);
}

void __log_decode_DICTPOS(long dictpos) {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, "DICTPOS %2ld | ", dictpos);
    fclose(log_lz77_vpassages);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "a");
    fputs(" ", decompress_bitview);
    fputs(__tobinary(11, dictpos), decompress_bitview);
    fclose(decompress_bitview);
}

void __log_decode_LEN(long len) {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, "DECODE_LEN %3ld | ", len);
    fclose(log_lz77_vpassages);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "a");
    fputs(" ", decompress_bitview);
    fputs(__tobinary(4, len), decompress_bitview);
    fclose(decompress_bitview);
}

void __log_decode_PRINT_DICTIONARY_REC() {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, "DICT_RECOVER ");
    fclose(log_lz77_vpassages);
}

void __log_decode_PRINT_RECOVERED_CHARS(unsigned char chrec) {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, "%c", chrec);
    fclose(log_lz77_vpassages);
}

void __log_decode_NEW_CHAR(unsigned char newchar) {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, " | NEWCHAR %c %d\n", newchar, newchar);
    fclose(log_lz77_vpassages);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "a");
    fputs(" ", decompress_bitview);
    fputs(__tobinary(8, newchar), decompress_bitview);
    fputs("\n", decompress_bitview);
    fclose(decompress_bitview);
}

void __log_decode_NEW_DCHAR(unsigned char newchar) {
    log_lz77_vpassages = fopen(LOG_DIR "/" VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, "NEWCHAR %c %d\n", newchar, newchar);
    fclose(log_lz77_vpassages);

    decompress_bitview = fopen(LOG_DIR "/" POST_DECOMPRESSION_BITVIEW_FILENAME, "a");
    fputs(" ", decompress_bitview);
    fputs(__tobinary(8, newchar), decompress_bitview);
    fputs("\n", decompress_bitview);
    fclose(decompress_bitview);
}