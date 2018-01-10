//
// Created by Enrico on 28.12.17.
//

#ifndef LZ77AC_ENC_H
#define LZ77AC_ENC_H
void initcompressor(char source[], char destination[]);
void runcompression(unsigned char next_byte);
void terminatecompression();
#endif //LZ77AC_ENC_H
