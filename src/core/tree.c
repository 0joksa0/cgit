#include "tree.h"
#include "blob.h"
#include "hash.h"
#include "ignore.h"
#include "main.h"
#include "object.h"

#include <dirent.h>
#include <errno.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned char* tree(char* dir)
{
    struct dirent** entryList = NULL;
    FILE* file = NULL;
    FILE* object = NULL;
    char* treeData = NULL;
    unsigned char* blob = NULL;
    unsigned char* hash_bin = NULL;
    unsigned char* compressedData = NULL;
    char* objectPath = NULL;
    unsigned char* entryHash = NULL;

    int fd = -1;
    int n = 0;

    if (!dir)
        dir = ".";

    char template[] = "./.mygit/tree_XXXXXX";
    n = scandir(dir, &entryList, NULL, alphasort);
    if (n < 0) {
        fprintf(stderr, "Failed to scan directory.\n");
        goto cleanup;
    }

    fd = mkstemp(template);
    if (fd < 0) {
        fprintf(stderr, "Failed to create temp file.\n");
        goto cleanup;
    }
    unlink(template);

    file = fdopen(fd, "w+b");
    if (!file) {
        fprintf(stderr, "Failed to open temp file stream.\n");
        goto cleanup;
    }

    for (int i = 0; i < n; i++) {
        char* name = entryList[i]->d_name;
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dir, name);

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strcmp(name, ".git") == 0 || strcmp(name, ".mygit") == 0 || isIgnored(path)) {
            free(entryList[i]);
            continue;
        }

        if (entryList[i]->d_type == DT_DIR) {
            entryHash = tree(path);
            if (!entryHash)
                goto cleanup;
            fprintf(file, "%s %s%c", DIR_CD, name, '\0');
            fwrite(entryHash, 1, 20, file);
        } else if (entryList[i]->d_type == DT_REG) {
            entryHash = createBlob(path);
            if (!entryHash)
                goto cleanup;
            fprintf(file, "%s %s%c", FILE_CD, name, '\0');
            fwrite(entryHash, 1, 20, file);
        } else {
            free(entryList[i]);
            continue;
        }

        free(entryHash);
        entryHash = NULL;
        free(entryList[i]);
    }

    free(entryList);
    entryList = NULL;

    fflush(file);
    fseek(file, 0, SEEK_END);
    size_t treeSize = ftell(file);
    rewind(file);

    treeData = malloc(treeSize);
    if (!treeData) {
        fprintf(stderr, "Memory allocation failed (treeData).\n");
        goto cleanup;
    }

    fread(treeData, 1, treeSize, file);
    fclose(file);
    file = NULL;

    size_t headerSize = snprintf(NULL, 0, "tree %zu", treeSize) + 1;
    size_t blob_size = headerSize + treeSize;

    blob = malloc(blob_size);
    if (!blob) {
        fprintf(stderr, "Memory allocation failed (blob).\n");
        goto cleanup;
    }

    snprintf((char*)blob, headerSize, "tree %zu", treeSize);
    memcpy(blob + headerSize, treeData, treeSize);

    hash_bin = malloc(SHA_DIGEST_LENGTH);
    if (!hash_bin) {
        fprintf(stderr, "Memory allocation failed (hash_bin).\n");
        goto cleanup;
    }

    char hash_str[SHA_DIGEST_LENGTH * 2 + 1];
    hash(hash_bin, hash_str, blob, blob_size);

    printf("file: %s\n", hash_str);

    objectPath = createObjectPath(hash_str);
    if (!objectPath) {
        fprintf(stderr, "createObjectPath failed.\n");
        goto cleanup;
    }

    object = fopen(objectPath, "wb");
    if (!object) {
        fprintf(stderr, "Error opening object file: %s\n", strerror(errno));
        goto cleanup;
    }

    unsigned long total_out = 0;
    compressedData = zlib_compress(blob, blob_size, &total_out);
    if (!compressedData) {
        fprintf(stderr, "Compression failed.\n");
        goto cleanup;
    }

    fwrite(compressedData, 1, total_out, object);

    fclose(object);
    free(treeData);
    free(blob);
    free(compressedData);
    free(objectPath);
    return hash_bin;

cleanup:
    if (file)
        fclose(file);
    if (object)
        fclose(object);
    if (entryList) {
        for (int i = 0; i < n; ++i)
            free(entryList[i]);
        free(entryList);
    }
    free(treeData);
    free(blob);
    free(compressedData);
    free(objectPath);
    free(hash_bin);
    if (entryHash) {
        free(entryHash);
    }
    return NULL;
}



unsigned char* writeTreeFromIndex(){

    return NULL;
}
