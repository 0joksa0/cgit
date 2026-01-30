#include "init.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void init()
{
    if (mkdir(".mygit", 0755) == -1 || mkdir(".mygit/objects", 0755) == -1 || mkdir(".mygit/refs", 0755) == -1 || mkdir(".mygit/refs/heads", 0755) == -1) {
        fprintf(stderr, "Failed to create directories: %s\n", strerror(errno));
        exit(1);
    }

    FILE* headFile = fopen(".mygit/HEAD", "w");
    if (headFile == NULL) {
        fprintf(stderr, "Failed to create .mygit/HEAD file: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(headFile, "ref: refs/heads/main\n");
    fclose(headFile);

    FILE* headIndex = fopen(".mygit/index", "w");
    if (headIndex == NULL) {
        fprintf(stderr, "Failed to create .mygit/index file: %s\n", strerror(errno));
        exit(1);
    }
    fclose(headIndex);

    FILE* headMain = fopen(".mygit/refs/heads/main", "w");
    if (headMain == NULL) {
        fprintf(stderr, "Failed to create .mygit/refs/heads/main file: %s\n", strerror(errno));
        exit(1);
    }
    fclose(headMain);

    printf("Initialized git directory\n");
}
