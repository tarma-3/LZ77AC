//
// Created by Enrico on 28.12.17.
//

#ifndef LZ77AC_DLOG_H
#define LZ77AC_DLOG_H
void __init_dec_log(char *source);

void __log_decode_FLAG1_FOUND();

void __log_decode_FLAG0_FOUND();

void __log_decode_DICTPOS(long dictpos);

void __log_decode_LEN(long len);

void __log_decode_PRINT_DICTIONARY_REC();

void __log_decode_PRINT_RECOVERED_CHARS(unsigned char chrec);

void __log_decode_NEW_CHAR(unsigned char newchar);

void __log_decode_NEW_DCHAR(unsigned char newchar);
#endif //LZ77AC_DLOG_H
