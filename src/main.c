#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "init.h"
#include "blobCommands.h"
#include "treeCommands.h"

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 2) {
        fprintf(stderr, "Usage: ./your_program.sh <command> [<args>]\n");
        return 1;
    }

    const char* command = argv[1];
    char** params = (argv + 2);

    if (strcmp(command, "init") == 0) {
        init();
    } else if (strcmp(command, "cat-file") == 0) {
       catBlob(params);
    } else if (strcmp(command, "hash-object") == 0) {
        blob(params);
    } else if (strcmp(command, "ls-tree") == 0) {
        listTree(params);
    } else if (strcmp(command, "write-tree") == 0) {
        writeTree();
    } else {
        fprintf(stderr, "Unknown command %s\n", command);
        return 1;
    }

    return 0;
}
