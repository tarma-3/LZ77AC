//
// Created by Enrico on 09/10/2017.
//

#ifndef LZ77AC_LZ77_H
#define LZ77AC_LZ77_H
extern const unsigned int DICTIONARY_SIZE;
extern const unsigned int LOOKAHEADB_SIZE;
extern int DEBUG_ENABLED;
char *lz77_encode(char source[]);
char *lz77_decode(char args[]);
#endif //LZ77AC_LZ77_H
