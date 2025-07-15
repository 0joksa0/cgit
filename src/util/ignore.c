#include "main.h"
#include "ignore.h"

#include <fnmatch.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool isIgnored(char* path)
{
    FILE* f = fopen(".gitignore", "r");
    if (!f)
        return false;
    char* normPath = path;
    if (strncmp(path, "./", 2) == 0) {
        normPath += 2;
    }
    char pat[256];
    while (fgets(pat, sizeof pat, f)) {

        pat[strcspn(pat, "\r\n")] = '\0';

        pat[strcspn(pat, "/")] = '\0';
        if (*pat == '\0' || *pat == '#')
            continue;

        char extended[300];
        if (pat[strlen(pat) - 1] == '/') {
            snprintf(extended, sizeof extended, "%s*", pat);
        } else {
            strncpy(extended, pat, sizeof extended);
        }

        if (fnmatch(extended, normPath, FNM_PATHNAME) == 0) {
            printf("ignored %s (matched %s)\n", normPath, extended);
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}
