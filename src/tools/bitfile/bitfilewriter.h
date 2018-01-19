#include <stdio.h>
typedef struct _bitfile BitfileWriter;
typedef enum _bf_mode {
    BF_READ, BF_WRITE
} BF_MODE;

BitfileWriter *newBitFileWriter(const char *filename);
int flushCloseBitfileWriter(BitfileWriter *fp);

void write_bit(unsigned int bit, BitfileWriter *fp);
void write_bits(unsigned int nofbitstow, unsigned int bitstow, BitfileWriter *fp);


