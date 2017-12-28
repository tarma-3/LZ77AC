//
// Created by Enrico on 09/10/2017.
//

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include "lz77.h"
#include "../tools/cmdef.h"
#include "../tools/charcb/charcb.h"
#include "../tools/file/file.h"
#include "../tools/kmp/kmpsearch.h"
#include "../tools/bytestr/bytestr.h"
#include "../tools/bitfile/bitfilewriter.h"
#include "../tools/bitfile/bitfilereader.h"

#define FLAG_SIZE 1
#define DICTIONARY_SIZE 30
#define LOOKAHEADB_SIZE 16
#define CHAR_SIZE 8


#if DEBUG_LZ77_LOG
#define COPY_CHECK_FILENAME "log_lz77-copy-check"
#define SETTINGS_LOG_FILENAME "log_lz77-settings.txt"
#define LOG_DIR "./log"
char VPASSAGES_FILENAME[60];
FILE *log_lz77;
FILE *log_lz77_vpassages;
FILE *log_lz77_dcwrite;
FILE *log_lz77_settings;
FILE *log_lz77_writebin;
size_t extlen;
#endif


CircularBuffer *dictionary;
// Compressed/Uncompressed output
BitfileWriter *output_file;
ByteString *lookahead_buffer;

typedef struct _tern {
    long flag;
    long dictpos;
    long len;
    unsigned char c;
} Tern;

/* _______________FUNZIONI DI DEBUG/LOG_______________
 * Funzioni fini al debug, non verranno compilati
 * nella release.
 * Le funzioni di debug hanno un doppio trattino basso
 * come inizio del nome.
 */
#if DEBUG_LZ77_LOG

void __init_lzz_log(char *source) {

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

    if(strcmp(get_filename_ext(source),".press")==0){
        char tmp[]=LOG_DIR "/log_lz77-vpassages_dec.txt";
        strcpy(VPASSAGES_FILENAME,tmp);
    } else{
        char tmp[]=LOG_DIR "/log_lz77-vpassages_cmp.txt";
        strcpy(VPASSAGES_FILENAME,tmp);
    }
    //strcpy(&VPASSAGES_FILENAME,LOG_DIR "/log_lz77-vpassages_dec.txt");

    /* Creo una stringa per contenere il nome del _file di log
     * che si occupa del copy-check, ovvero si assicura che il
     * _file venga letto correttamente. Questo _file risultato
     * quindi lo stesso contenuto del _file sorgente.
     */
    char log_copycheck_filename[strlen(LOG_DIR) + 1 + strlen(COPY_CHECK_FILENAME) + get_extlen(source) + 1];
    {
        log_copycheck_filename[0] = '\0';
        strcat(log_copycheck_filename, LOG_DIR);
        strcat(log_copycheck_filename, "/");
        strcat(log_copycheck_filename, COPY_CHECK_FILENAME);
        strcat(log_copycheck_filename, get_filename_ext(source));
    }
    // Creo e apro il _file di log
    log_lz77 = fopen(log_copycheck_filename, "wb+");
    log_lz77_dcwrite = fopen(LOG_DIR "/log_lz77-writedc.txt", "wb+");
    fclose(log_lz77_dcwrite);

    log_lz77 = fopen(log_copycheck_filename, "wb+");
    log_lz77_writebin = fopen(LOG_DIR "/log_lz77-writebin.txt", "wb+");
    fclose(log_lz77_writebin);

    log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "wb+");
    fputs("CALL _run_encode(char/numvalue)\n", log_lz77_vpassages);
    fputs("Match found [dict_id/offset]\n", log_lz77_vpassages);
    fputs(" --- \n", log_lz77_vpassages);
    fclose(log_lz77_vpassages);
}

#endif


/* ______FUNZIONI DI COMPRESSIONE/DECOMPRESSIONE______
 * Le funzioni private oltre a non essere visibili nel
 * _file header sono nominate con un trattino basso
 * prima del nome.
 */
// __________FUNZIONI PRINCIPALI
void _init_codec(char source[], char destination[]);

void _run_encode(unsigned char next_byte);

void _run_decode(BitfileReader *bf_reader, FILE *output_file);

// __________FUNZIONI DI SUPPORTO
void _finalmatchfound(size_t *lookahbf_id, long *dictmatch_id, long *backup_dm_id, size_t *offset);

int _log2(int x);


char *lz77_encode(char args[]) {
    //TODO: Parametrizzare questa chiamata
    _init_codec(args, "./output.press");
    stream_file_to(args, _run_encode);
    return "f";
}

BitfileReader *ternparser;

char *lz77_decode(char args[]) {
    //TODO: Parametrizzare questa chiamata
    _init_codec("./output.press", "./output");
    BitfileReader *bf_reader = newBitfileReader("./output.press");
    FILE *output_file = fopen("./output", "w");
    _run_decode(bf_reader, output_file);
    return "f";
}

void _init_codec(char source[], char destination[]) {
    dictionary = uccb_init(DICTIONARY_SIZE);
    lookahead_buffer = ba_init(LOOKAHEADB_SIZE);

#if DEBUG_LZ77_LOG
    __init_lzz_log(source);
#endif
    output_file = bfopen(destination);
}

void _run_encode(unsigned char next_byte) {
#if DEBUG_LZ77_LOG
    /* Nel caso sia in modalità debug stampo il prossimo byte letto. Sia sottoforma di carattere (per
     * facilitarne l'interpretazione) sia il suo valore numerico.
     * */
    log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
    fprintf(log_lz77_vpassages, "CALL _run_encode(%c/%3d)\n", next_byte, next_byte);
    fclose(log_lz77_vpassages);
#endif
    static size_t offset = 0;
    static size_t lookahbf_id = 0;
    /* lookahbf_id è una variabile che si occupa di tenere in memoria l'indice dell'ultimo carattere all'interno del
     * lookahead_buffer per la quale è stata trovata una ricorrenza nel dictionary.
     * ES:  [dictionary][lookahead_buffer]
     *      ===================================
     *      [abxabc(...)][abc(...)]
     *        ^           ^
     *      In questo momento lookahbf_id = 1
     *      -----------------------------------
     *      [abxabcghil][abc(...)]
     *            ^        ^
     *      In questo momento lookahbf_id = 2
     */

    static long dictmatch_id = -1;
    /* dictmatch_id è una variabile che si occupa di tenere in memoria in quale posizione nel dictionary è stata trovata
     * un'occorrenza
     */

    static long backup_dm_id = -1;
    backup_dm_id = dictmatch_id;
    /* backup_dm_id è una variabile "di servizio" temporanea che fa da backup della posizione del match
     * precedente nel dictionary (ovvero il contenuto di dictmatch_id).
     */


    bs_set(lookahead_buffer, lookahbf_id, next_byte);
    // Inserirsco un nuovo carattere nel lookahead buffer

    /*
     * Caso 1, precedentemente vi è stato un match e vogliamo verificare se il carattere successivo del lookahead_buffer
     * è uguale a quello successivo contenuto in dictionary.
     * Per verificare se prima vi è stato un match allora deve valere (lookahbf_id  ≠  0)
     * Inoltre il dictionary deve avere a disposizione un carattere successivo
     * In ultimo possiamo procedere al confronto se il prossimo carattere di dictionary coincide con lookahead_buffer
     * Nota: lookahbf_id è già stato incrementato per essere sull'indice da confrontare
     *
     * In questo "if" il caso in cui il confronto carattere per carattere ha dato esito positivo
     */
    if (lookahbf_id && uccb_hasnext(dictionary) && uccb_next(dictionary) == bs_get(lookahead_buffer, lookahbf_id)) {
#if DEBUG_LZ77_LOG
        log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
        char t[bs_capacity(lookahead_buffer)];
        fprintf(log_lz77_vpassages, "   +   found %2ld/%2ld, pattern [%s]\t(consecutive)\n", dictmatch_id,offset, bs_string(lookahead_buffer, t));
        fclose(log_lz77_vpassages);
#endif
        if (++lookahbf_id == LOOKAHEADB_SIZE) {
#if DEBUG_LZ77_LOG
            log_lz77_vpassages = fopen(LOG_DIR "/log_lz77-vpassages.txt", "a");
            char t[bs_capacity(lookahead_buffer)];
            fprintf(log_lz77_vpassages, "DEFINITIVE   %2ld/%2ld, pattern [%s] (due look-ahead buffer overflow)\n\n",
                    backup_dm_id,offset, bs_string(lookahead_buffer, t));
            fclose(log_lz77_vpassages);
#endif
            _finalmatchfound(&lookahbf_id, &dictmatch_id, &backup_dm_id, &offset);
        }
    }/*
    * Caso 2, il confronto con il carattere seguente (caso 1) ha dato esito negativo e si vuole verificare se più
    * avanti nel dictionary vi è una occorrenza che soddisfa il lookahead_buffer.
    * In questo "if" il caso in cui questa ricerca ha dato ESITO POSITIVO.
    */
    else if ((dictmatch_id = upmatch(lookahead_buffer, dictionary, lookahbf_id)) != -1) {
        offset = uccb_nofchars(dictionary) - lookahbf_id - dictmatch_id;
#if DEBUG_LZ77_LOG
        log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
        if (backup_dm_id != -1) {
            fprintf(log_lz77_vpassages, "   *   Old match discharged, better option found\n");
        }
        fclose(log_lz77_vpassages);
#endif
        // Incrementiamo lookahbf_id in modo che alla prossima chiamata è già settato correttamente
        if (++lookahbf_id == LOOKAHEADB_SIZE) {
#if DEBUG_LZ77_LOG
            log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
            char t[bs_capacity(lookahead_buffer)];
            fprintf(log_lz77_vpassages, "DEFINITIVE   %2ld/%2ld, pattern [%s] (due look-ahead buffer overflow)\n",
                    backup_dm_id, offset, bs_string(lookahead_buffer, t));
            fclose(log_lz77_vpassages);
#endif
            _finalmatchfound(&lookahbf_id, &dictmatch_id, &backup_dm_id, &offset);
        };
#if DEBUG_LZ77_LOG
        log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
        char t[bs_capacity(lookahead_buffer)];
        fprintf(log_lz77_vpassages, " Match found %2ld/%2ld, pattern [%s]\n", dictmatch_id, offset,
                bs_string(lookahead_buffer, t));
        fclose(log_lz77_vpassages);
#endif
    }
        /*
         * Caso 3, il confronto con il carattere seguente (caso 1) ha dato esito negativo e si vuole verificare se più
         * avanti nel dictionary vi è una occorrenza che soddisfa il lookahead_buffer.
         * In questo "if" il caso in cui questa ricerca ha dato ESITO NEGATIVO.
         */

    else {
#if DEBUG_LZ77_LOG
        log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
        char t[bs_capacity(lookahead_buffer)];
        fprintf(log_lz77_vpassages, "DEFINITIVE   %2ld/%2ld, pattern [%s]\n\n", backup_dm_id,offset,
                bs_string(lookahead_buffer, t));
        fclose(log_lz77_vpassages);
#endif
        dictmatch_id = backup_dm_id;
        _finalmatchfound(&lookahbf_id, &dictmatch_id, &backup_dm_id, &offset);
    }
    // Una volta fatti i controlli posso inserire in dictionary il valore passato come argomento 'next_byte'
    uccb_push(next_byte, dictionary);
#if DEBUG_LZ77_LOG
    log_lz77_vpassages = fopen(VPASSAGES_FILENAME, "a");
    fprintf(log_lz77, "%c", next_byte);
    fclose(log_lz77_vpassages);
#endif
}

void _finalmatchfound(size_t *lookahbf_id, long *dictmatch_id, long *backup_dm_id, size_t *offset) {

#if DEBUG_LZ77_LOG
    char binaryrap[25];
    binaryrap[0] = '\0';
#endif

    if ((bs_getlen(lookahead_buffer) - 1)) {
#if DEBUG_LZ77_LOG
        log_lz77_dcwrite = fopen(LOG_DIR "/log_lz77-writedc.txt", "a");
        fprintf(log_lz77_dcwrite, "[1,%4zu,%4zu,", *offset, bs_getlen(lookahead_buffer));
        fclose(log_lz77_dcwrite);
        binaryrap[0] = '1';
        for (int i = 0; i < 11; ++i) {
            binaryrap[1 + i] = '0' + (((unsigned int) *offset >> (10 - i)) & 1);
        }
        for (int i = 0; i < 4; ++i) {
            binaryrap[12 + i] = '0' + (((unsigned int) bs_getlen(lookahead_buffer) >> (3 - i)) & 1);
        }
        for (int i = 0; i < 8; ++i) {
            binaryrap[16 + i] =
                    '0' + (((unsigned int) bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1) >> (7 - i)) & 1);
        }
        binaryrap[24] = '\0';
#endif
        write_bit(1, output_file);
        write_bits(11, *offset, output_file);
        write_bits(4, bs_getlen(lookahead_buffer) - 1, output_file);
    } else {

#if DEBUG_LZ77_LOG
        log_lz77_dcwrite = fopen(LOG_DIR "/log_lz77-writedc.txt", "a");
        fprintf(log_lz77_dcwrite, "[0, ");
        fclose(log_lz77_dcwrite);
        binaryrap[0] = '0';
        for (int i = 0; i < 8; ++i) {
            binaryrap[1 + i] =
                    '0' + (((unsigned int) bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1) >> (7 - i)) & 1);
        }
        binaryrap[9] = '\0';
#endif
        write_bit(0, output_file);
    }
    write_bits(8, bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1), output_file);

#if DEBUG_LZ77_LOG
    log_lz77_dcwrite = fopen(LOG_DIR "/log_lz77-writedc.txt", "a");
    fprintf(log_lz77_dcwrite, "%c] %s\n", bs_get(lookahead_buffer, bs_getlen(lookahead_buffer) - 1), binaryrap);
    fclose(log_lz77_dcwrite);

    log_lz77_writebin = fopen(LOG_DIR "/log_lz77-writebin.txt", "a");
    fprintf(log_lz77_writebin, "%s", binaryrap);
    fclose(log_lz77_writebin);
#endif

    uccb_reset(dictionary);
    *lookahbf_id = 0;
    *dictmatch_id = -1;
    *backup_dm_id = -1;
    *offset = 0;
    bs_setlen(lookahead_buffer, 0);
}

typedef enum _decodestatus {
    DECODE_FLAG, DECODE_DICTPOS, DECODE_LEN, DECODE_CHAR, DECODE_DCHAR
} DecodeStatus;

void _run_decode(BitfileReader *bf_reader, FILE *output_file) {
    static DecodeStatus decodeStatus = DECODE_FLAG;
    static Tern tern;
    int isEOF = 0;
    while (isEOF == 0) {
        switch (decodeStatus) {
            case DECODE_FLAG:
                tern.flag = read_bits(1, bf_reader);
                if (tern.flag)
                    decodeStatus = DECODE_DICTPOS;
                else
                    decodeStatus = DECODE_DCHAR;
                break;
            case DECODE_DICTPOS:
                tern.dictpos = read_bits(11, bf_reader);
                decodeStatus = DECODE_LEN;
                break;
            case DECODE_LEN:
                tern.len = read_bits(4, bf_reader);
                decodeStatus = DECODE_CHAR;
                break;
            case DECODE_CHAR:
                tern.c = (unsigned char) read_bits(8, bf_reader);
                // Mi posiziono tanti passi indietro quanti mi è stato detto da dictpos
                size_t dictsize = uccb_nofchars(dictionary);
                cb_setid(dictsize - (unsigned int) tern.dictpos, dictionary);
                for (int i = 0; i < tern.len; ++i) {
                    unsigned char chrec = uccb_pointed(dictionary);
                    uccb_push(chrec, dictionary);
                    uccb_next(dictionary);
                    fprintf(output_file, "%c", chrec);
#if DEBUG_LZ77_LOG
                    log_lz77_dcwrite = fopen(LOG_DIR "/log_lz77-writedc.txt", "a");
                    fprintf(log_lz77_dcwrite, "%c", chrec);
                    fclose(log_lz77_dcwrite);
#endif
                }
                fprintf(output_file, "%c", tern.c);

#if DEBUG_LZ77_LOG
                log_lz77_dcwrite = fopen(LOG_DIR "/log_lz77-writedc.txt", "a");
                fprintf(log_lz77_dcwrite, "%c", tern.c);
                fclose(log_lz77_dcwrite);
#endif
                uccb_push(tern.c, dictionary);

                decodeStatus = DECODE_FLAG;
                isEOF = read_checkEOF(bf_reader);
                break;
            case DECODE_DCHAR:
                tern.c = (unsigned char) read_bits(8, bf_reader);
                fprintf(output_file, "%c", tern.c);
                uccb_push(tern.c, dictionary);

                decodeStatus = DECODE_FLAG;
                isEOF = read_checkEOF(bf_reader);
                break;
            default:
                fprintf(stderr, "_run_decode exception");
                exit(23);
        }
    }

    /*

    static int counter = 0;
    static char s[5] = {'\0'};

    static Tern t;

    s[counter] = next_byte;
    s[counter + 1] = '\0';

    switch (decstatus) {
        case dict:
            if (++counter == 4) {
                counter = 0;
                decstatus = lookahead;

                t.a = strtol(s, NULL, 10);
                s[0] = '\0';
            }
            break;
        case lookahead:
            if (++counter == 2) {
                counter = 0;
                decstatus = character;
                t.b = strtol(s, NULL, 10);
                s[0] = '\0';
            }
            break;
        case character:
            if (++counter == 1) {
                counter = 0;
                decstatus = dict;
                t.c = (unsigned char) s[0];
                s[0] = '\0';
                _rendertern(t);
            }
            break;
        default:
            fprintf(stderr, "Error: RunEncode undefined state");
            exit(0);
    }
    */
}

/*
void _rendertern(Tern tern) {
    //printf("[%4ld,%2ld,%c] -> [", tern.a, tern.b, tern.c);
    if (tern.a == -1) {
        //printf("%c]\n", tern.c);
        fprintf(output_file, "%c", tern.c);
        uccb_push(tern.c, dictionary);
    } else {
        cb_setid(uccb_nofchars(dictionary) - (unsigned int) tern.a, dictionary);
        for (int i = 0; i < tern.b; ++i) {
            unsigned char chrec = uccb_pointed(dictionary);
            //TODO: Continuare da qui
            uccb_push(chrec, dictionary);
            uccb_next(dictionary);
            //printf("%c", chrec);
            fprintf(output_file, "%c", chrec);
        }
        //printf("%c]\n", tern.c);
        fprintf(output_file, "%c", tern.c);
        uccb_push(tern.c, dictionary);
    }

}
*/



//TODO: Spostare questa funzione
// log2 viene garantito dallo stdC11
int _log2(int x) {
    return (int) (log(x) / log(2));
}