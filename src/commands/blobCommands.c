#include "blobCommands.h"
#include "object.h"
#include "blob.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void catBlob(char** params)
{
    int status = EXIT_FAILURE;

    if (params == NULL) {
        fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>]\n");
        exit(1);
    }
    if (params[0] == NULL || strcmp(params[0], "-p")) {
        fprintf(stderr, "Invalid option.\n");
        exit(1);
    }
    if (params[1] == NULL || strlen(params[1]) != 40) {
        fprintf(stderr, "Invalid object name : Must contain 40 characters.\n");
        exit(1);
    }
    for (int i = 0; i < 40; i++) {
        if (!isxdigit(params[1][i])) {
            fprintf(stderr,
                "Invalid object name : Must contain only hexadecimal digits.\n");
            exit(1);
        }
    }
    FILE* file = NULL;
    unsigned char* compressedData = NULL;
    unsigned char* decompressedData = NULL;
    char* objectPath = createObjectPath(params[1]); 

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
    decompressedData = zlib_decompress(compressedData,fileSize, &total_out);
    if (!decompressedData) {
        goto cleanup;
    }
    char* context = strchr(decompressedData, '\x00');
    context++;
    fwrite(context, 1, strlen(context), stdout);
    status = EXIT_SUCCESS;


cleanup:
    if (file) {
        fclose(file);
    }
    free(compressedData);
    free(decompressedData);
    free(objectPath);
    if (status != EXIT_SUCCESS) {
        exit(status);
    }
}

void blob(char* params[])
{
    if (!params || !params[0] || strcmp(params[0], "-w") != 0) {
        fprintf(stderr, "Usage: ./your_program.sh hash-object -w <file>\n");
        exit(1);
    }
    if (!params[1]) {
        fprintf(stderr, "Error: File name missing.\n");
        exit(1);
    }

    free(createBlob(params[1]));
}
