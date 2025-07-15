#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
unsigned char* zlib_decompress(unsigned char* compressedData, size_t fileSize, unsigned long* total_out);

unsigned char* zlib_compress(unsigned char* decompressedData, size_t fileSize, unsigned long* total_out);

char* createObjectPath(char* hash_str);

#endif // !OBJECT_H
