#include "blob.h"
#include "main.h"
#include "read.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

void addToInexCommand(char** params)
{

    if (params == NULL || params[0] == NULL) {
        fprintf(stderr, "Usage: mygit add <files>\n");
        exit(1);
    }

    FILE* index = fopen(".mygit/index", "a");
    if (!index) {
        perror("Could not open index file");
        exit(1);
    }

    while (*params != NULL) {
        char* path = *params;

        struct stat st;
        if (stat(path, &st) < 0) {
            fprintf(stderr, "Cannot stat file: %s\n", path);
            params++;
            continue;
        }

        char mode[7];
        if (S_ISREG(st.st_mode)) {
            if (st.st_mode & S_IXUSR)
                strcpy(mode, EXECUTABLE_CD);
            else
                strcpy(mode, FILE_CD);
        } else if (S_ISLNK(st.st_mode)) {
            strcpy(mode, SYMBOLIC_CD);
        } else {
            return;
        }

        if (fileContainsString(".mygit/index", path) == 0) {
            printf("dodat");
            unsigned char* sha1_hex = createBlob(path);
            if (!sha1_hex) {
                fprintf(stderr, "Blob failed for: %s\n", path);
                params++;
                continue;
            }
            fprintf(index, "%s %s %s\n", mode, sha1_hex, path);

            free(sha1_hex);
        }

        params++;
    }

    fclose(index);
}
