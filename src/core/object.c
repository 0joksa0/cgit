#include "main.h"
#include "object.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#define ZLIB_BUFFER_SIZE 16384


unsigned char* zlib_decompress(unsigned char* compressedData, size_t fileSize, unsigned long* total_out)
{
    unsigned char* decompressedData = malloc(sizeof(unsigned char) * ZLIB_BUFFER_SIZE);
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    stream.next_in = compressedData;
    stream.avail_in = fileSize;
    stream.next_out = decompressedData;
    stream.avail_out = sizeof(unsigned char) * ZLIB_BUFFER_SIZE;

    if (inflateInit(&stream) != Z_OK) {
        fprintf(stderr, "Failed to initialize zlib.\n");
        free(decompressedData);
        return NULL;
    }

    if (inflate(&stream, Z_NO_FLUSH) != Z_STREAM_END) {
        fprintf(stderr, "Failed to decompress data.\n");
        inflateEnd(&stream);
        free(decompressedData);
        return NULL;
    }

    inflateEnd(&stream);
    *total_out = stream.total_out;
    decompressedData[*total_out] = '\0';

    
    return decompressedData;
}

unsigned char* zlib_compress(unsigned char* decompressedData, size_t fileSize, unsigned long* total_out)
{

    unsigned char* compressedData = malloc(sizeof(unsigned char) * ZLIB_BUFFER_SIZE);
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    deflateInit(&stream, Z_BEST_COMPRESSION);

    stream.next_in = decompressedData;
    stream.avail_in = fileSize;

    stream.next_out = compressedData;
    stream.avail_out = sizeof(unsigned char) * ZLIB_BUFFER_SIZE;

    deflate(&stream, Z_FINISH);
    deflateEnd(&stream);
    *total_out = stream.total_out;

    return compressedData;
}

char* createObjectPath(char* hash_str)
{
    char* dirPath = malloc(64);
    char* objectPath = malloc(128);
    if (!dirPath || !objectPath) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    snprintf(dirPath, 64, "%s/%.2s", DOT_GIT_OBJ, hash_str);
    mkdir(dirPath, 0755); 

    snprintf(objectPath, 128, "%s/%.2s/%s", DOT_GIT_OBJ, hash_str, hash_str + 2);

    free(dirPath); 
    return objectPath; 
}
