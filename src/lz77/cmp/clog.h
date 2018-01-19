//
// Created by Enrico on 28.12.17.
//

#ifndef LZ77AC_CLOG_H
#define LZ77AC_CLOG_H
#include "../../tools/bytestr/bytestr.h"
void __init_cmp_log(char *source);

void __log_lz77_vpassages_run_encode_CALL(unsigned char next_byte);

void __log_lz77_vpassages_CONSECUTIVE_MATCH(long dictmatch_id, size_t offset, ByteString *lookahead_buffer);

void __log_lz77_vpassages_DEFINITIVE_MATCH(long dictmatch_id, size_t offset, ByteString *lookahead_buffer);

void __log_lz77_vpassages_BETTER_MATCH(long dictmatch_id);

void __log_lz77_vpassages_MATCH_FOUND(long dictmatch_id, size_t offset, ByteString *lookahead_buffer);

void _log_COPYCHECK(unsigned char next_byte);

void __log_lz77_init_BINARY_RAP();

void __log_lz77_dcwrite_FLAG1(size_t offset, ByteString *lookahead_buffer);
void __log_lz77_dcwrite_FLAG1_E(size_t offset, size_t pattern_len);

void __log_lz77_dcwrite_FLAG0(ByteString *lookahead_buffer);

void __log_lz77_dcwrite_NEWCHAR(ByteString *lookahead_buffer);

void __log_lz77_dcwrite_TERMINATE();

void __log_bitopen(unsigned char newchar);
#endif //LZ77AC_CLOG_H
