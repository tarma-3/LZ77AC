//
// Created by Enrico on 28.12.17.
//

#include "enc.h"
#include "../../tools/charcb/cb/cibuff.h"
#include "../../tools/bitfile/bitfilewriter.h"
#include "../../tools/bytestr/bytestr.h"
#include "../../tools/charcb/charcb.h"

#include "clog.h"
#include "../lz77.h"
#include "../../tools/kmp/kmp.h"

static CircularBuffer *dictionary;
static BitfileWriter *output_file;
static size_t offset = 0;
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
static size_t lookahbf_id = 0;
/* dictmatch_id è una variabile che si occupa di tenere in memoria in quale posizione nel dictionary è stata trovata
 * un'occorrenza
 */
static long dictmatch_id = -1;

bool lastwasfinalmatch = true;

const unsigned int DICTIONARY_SIZE;

void _finalmatchfound();

void initcompressor(char source[], char destination[]) {
    DEBUG_ENABLED = 1;
    dictionary = uccb_init(DICTIONARY_SIZE);
    kmp_init();
    output_file = newBitFileWriter(destination);
#if DEBUG_LZ77_LOG
    __init_cmp_log(source);
#endif
}

static long charswritten = 0;

void runcompression(unsigned char next_byte) {
#if DEBUG_LZ77_LOG
    if (DEBUG_ENABLED)__log_lz77_vpassages_run_encode_CALL(next_byte); // log chiamata metodo
#endif
    //TODO: DEBUG ONLY
  //  if (charswritten >= 50)
       // DEBUG_ENABLED = 1;
   // if (charswritten >= 50 + 50)
       // exit(0);
    charswritten++;
    //END OF DEBUG

    lastwasfinalmatch = false;

    long tmp_dm_id;
    kmp_addc(next_byte);
    //TODO: EXPERIMENTAL
    if (kmp_isfull()) {
        _finalmatchfound();
    }// Controllo se troviamo un match
    else if ((tmp_dm_id = kmp_match(dictionary)) == NMF) {
        _finalmatchfound();
    } else {
        dictmatch_id = tmp_dm_id;
    }
    uccb_push(next_byte, dictionary);
}


void _finalmatchfound() {
    lastwasfinalmatch = true;
    if (dictmatch_id != NMF) {
        write_bit(1, output_file);
        write_bits(11, (unsigned int) dictmatch_id, output_file);
        if (((kmp_patternlen() - 1) & 15) == 0) {
            fprintf(stderr,"Wrong pattern len");
            exit(0);
        }
        write_bits(4, (unsigned int) kmp_patternlen() - 1, output_file);
    } else {
        write_bit(0, output_file);
    }
    write_bits(8, (unsigned char) kmp_getlastc(), output_file);
    cb_reset(dictionary);
    kmp_reset();
    lookahbf_id = 0;
    dictmatch_id = -1;
    offset = 0;
}

void terminatecompression() {
    /* Quando ho finito la compressione posso essere capitato in 2 casi:
     * L'ultimo carattere ha causato un final match e quindi la compressione è completata
     * L'ultimo carattere non ha causato un final match e quindi si deve mandare in output una terna incompleta (senza
     *  il new char)
     */
    if (!lastwasfinalmatch) {
#if DEBUG_LZ77_LOG
        __log_lz77_dcwrite_FLAG1_E(offset, kmp_patternlen());
#endif
        write_bit(1, output_file);
        write_bits(11, (unsigned int) dictmatch_id, output_file);
        write_bits(4, (unsigned int) (kmp_patternlen()), output_file);
#if DEBUG_LZ77_LOG
        __log_lz77_dcwrite_TERMINATE();

#endif
    }
    flushCloseBitfileWriter(output_file);
}