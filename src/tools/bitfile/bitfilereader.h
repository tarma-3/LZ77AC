//
// Created by Enrico on 26.12.17.
//

#ifndef LZ77AC_BITFILEREADER_H
#define LZ77AC_BITFILEREADER_H
typedef struct _bitfiler BitfileReader;
BitfileReader *newBitfileReader(const char *path);
unsigned int read_bits(unsigned int bitsreq, BitfileReader *fp);
unsigned int read_checkEOF(BitfileReader *fp);
#endif //LZ77AC_BITFILEREADER_H
