#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* readHeadTarget()
{

    FILE* HEAD = fopen(".mygit/HEAD", "r");
    char* line = NULL;
    size_t len = 0;

    getline(&line, &len, HEAD);
    fclose(HEAD);
    line[strcspn(line, "\r\n")] = 0;

    if (strncmp(line, "ref: ", 5) == 0) {
        char* ref = strdup(line + 5);
        free(line);
        return ref;
    }

    char* sha1 = strdup(line);
    free(line);
    return sha1;
}

char* readTargetHash(char* target)
{
    FILE* file = fopen(target, "r");
    char* line = NULL;
    size_t len = 0;

    getline(&line, &len, file);
    fclose(file);
    line[strcspn(line, "\r\n")] = 0;

    char* sha = strdup(line);
    free(line);
    return sha;
}

int fileContainsString(char* filename, char* string){
    FILE* file = fopen(filename, "r");
    char* line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        if (strstr(line, string) != NULL) {
            printf("sadrzi vec");
            fclose(file);
            free(line);
            return 1;
        } 
    }

    fclose(file);
    free(line);
    return 0;
}
