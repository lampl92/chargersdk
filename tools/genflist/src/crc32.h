#ifndef CRC32_H_INCLUDED
#define CRC32_H_INCLUDED
#include <stdint.h>
int GetFileCrc32(FILE *fp, uint32_t *pulCrc32);

#endif // CRC32_H_INCLUDED
