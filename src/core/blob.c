#include "blob.h"
#include "hash.h"
#include "object.h"
#include <errno.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

unsigned char* createBlob(char* name)
{
    FILE* file = NULL;
    FILE* object = NULL;
    char* fileData = NULL;
    unsigned char* blob = NULL;
    unsigned char* compressedData = NULL;
    char* objectPath = NULL;
    unsigned char* hash_bit = NULL;

    int status = EXIT_FAILURE;

    file = fopen(name, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s: %s\n", name,
            strerror(errno));
        goto cleanup;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    fileData = malloc(fileSize);
    if (!fileData) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        goto cleanup;
    }

    fread(fileData, 1, fileSize, file);
    size_t headerSize = snprintf(NULL, 0, "blob %zu", fileSize) + 1;
    size_t blob_size = headerSize + fileSize;
    blob = malloc(blob_size * sizeof(unsigned char));
    if (!blob) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        goto cleanup;
    }

    snprintf(blob, headerSize, "blob %zu", fileSize);
    memcpy(blob + headerSize, fileData, fileSize);

    hash_bit = malloc(SHA_DIGEST_LENGTH);
    unsigned char hash_str[SHA_DIGEST_LENGTH * 2 + 1];

    hash(hash_bit, hash_str, blob, blob_size);

    printf("object hash hex: %s\n", hash_str);

    objectPath = createObjectPath(hash_str);

    /* printf("created file at %s", objectPah); */
    object = fopen(objectPath, "wb");
    if (!object) {
        fprintf(stderr, "Error: Could not create object file: %s\n",
            strerror(errno));
        goto cleanup;
    }

    unsigned long total_out = 0;
    compressedData = zlib_compress(blob, blob_size, &total_out);
    fwrite(compressedData, 1, total_out, object);
    status = EXIT_SUCCESS;

cleanup:
    if (file)
        fclose(file);
    if (object)
        fclose(object);
    free(fileData);
    free(blob);
    free(compressedData);
    free(objectPath);

    if (status != EXIT_SUCCESS) {
        free(hash_bit);
        exit(status);
    }

    return hash_bit;
}
