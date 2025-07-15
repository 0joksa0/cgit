#include "treeCommands.h"
#include "object.h"
#include "tree.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void listTree(char* params[])
{
    FILE* file = NULL;
    unsigned char* compressedData = NULL;
    unsigned char* decompressedData = NULL;
    char* objectPath = NULL;
    char* name = NULL;

    int status = EXIT_FAILURE;

    if (params == NULL) {
        fprintf(stderr, "Usage: ./your_program.sh ls-tree [<args>]\n");
        exit(1);
    }
    if (params[0] == NULL || strlen(params[0]) != 40) {
        fprintf(stderr, "Invalid object name : Must contain 40 characters.\n");
        exit(1);
    }
    for (int i = 0; i < 40; i++) {
        if (!isxdigit(params[0][i])) {
            fprintf(stderr,
                "Invalid object name : Must contain only hexadecimal digits.\n");
            exit(1);
        }
    }

    name = strdup(params[0]);

    objectPath = createObjectPath(name);

    file = fopen(objectPath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open object file : %s\n", strerror(errno));
        goto cleanup;
    }

    fseek(file, 0, SEEK_END);
    unsigned long fileSize = ftell(file);
    rewind(file);
    compressedData = malloc(fileSize);
    if (compressedData == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        goto cleanup;
    }
    fread(compressedData, 1, fileSize, file);

    unsigned long total_out = 0;
    decompressedData = zlib_decompress(compressedData, fileSize, &total_out);

    size_t out_len = total_out;
    unsigned char* p = decompressedData;
    unsigned char* end = decompressedData + out_len;

    unsigned char* hdr_end = memchr(p, '\0', end - p);
    if (!hdr_end) {
        fprintf(stderr, "Corrupted tree object\n");
    }
    p = hdr_end + 1;

    while (p < end) {
        unsigned char* sp = memchr(p, ' ', end - p);
        unsigned char* nul = sp ? memchr(sp + 1, '\0', end - (sp + 1)) : NULL;
        unsigned char* sha = nul ? nul + 1 : NULL;
        if (!sp || !nul || sha + 20 > end)
            break;

        printf("%.*s %.*s ", (int)(sp - p), p,
            (int)(nul - (sp + 1)), sp + 1);
        for (int i = 0; i < 20; ++i)
            printf("%02x", sha[i]);
        putchar('\n');

        p = sha + 20;
    }

    status = EXIT_SUCCESS;

cleanup:
    if (file)
        fclose(file);
    free(compressedData);
    free(decompressedData);
    free(objectPath);
    free(name);
    if (status != EXIT_SUCCESS) {
        exit(status);
    }
}

void writeTree()
{
    free(tree(NULL));
}
