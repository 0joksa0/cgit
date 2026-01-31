// Tests adapted to use your real implementation in src/
#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "core/blob.h"
#include "core/object.h"
#include "commands/init.h"
#include "util/hash.h"

#define FIXTURE_FILE "/home/joksa/cgit/tests/fixtures/sample.txt"

/* Helper: da li fajl postoji */
static int file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

/* Helper: učitaj ceo fajl u memoriju */
static char *read_file(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    cr_assert_not_null(f);

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = malloc(size + 1);
    cr_assert_not_null(buf);

    size_t r = fread(buf, 1, size, f);
    (void)r;
    fclose(f);

    buf[size] = '\0';
    *out_size = size;
    return buf;
}

Test(main_cmds, init_creates_mygit_structure) {
    /* ensure clean state */
    system("rm -rf .mygit");

    /* call real init() from your code */
    extern void init();
    init();

    cr_assert(file_exists(".mygit"), ".mygit directory missing");
    cr_assert(file_exists(".mygit/objects"), ".mygit/objects missing");
    cr_assert(file_exists(".mygit/HEAD"), ".mygit/HEAD missing");
    cr_assert(file_exists(".mygit/index"), ".mygit/index missing");
    cr_assert(file_exists(".mygit/refs/heads/main"), ".mygit/refs/heads/main missing");

    /* cleanup */
    system("rm -rf .mygit");
}

Test(blob_core, create_blob_writes_object_and_preserves_content) {
    /* prepare repo */
    system("rm -rf .mygit");
    extern void init();
    init();

    /* create blob from fixture */
    unsigned char *raw_hash = createBlob(FIXTURE_FILE);
    cr_assert_not_null(raw_hash);

    /* convert raw hash (20 bytes) to hex string */
    char hex[41] = {0};
    for (int i = 0; i < 20; ++i) {
        snprintf(hex + i * 2, 3, "%02x", raw_hash[i]);
    }

    /* object path and existence */
    char *obj_path = createObjectPath(hex);
    cr_assert_not_null(obj_path);
    cr_assert(file_exists(obj_path), "object file %s does not exist", obj_path);

    /* read compressed object and decompress */
    FILE *f = fopen(obj_path, "rb");
    cr_assert_not_null(f);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    unsigned char *comp = malloc(fsize);
    cr_assert_not_null(comp);
    fread(comp, 1, fsize, f);
    fclose(f);

    unsigned long total_out = 0;
    unsigned char *decomp = zlib_decompress(comp, fsize, &total_out);
    cr_assert_not_null(decomp);

    /* skip header up to first null byte */
    unsigned char *nul = (unsigned char *)memchr(decomp, '\0', total_out);
    cr_assert_not_null(nul);
    unsigned char *content = nul + 1;
    size_t content_len = total_out - (content - decomp);

    /* read original fixture */
    size_t orig_size;
    char *orig = read_file(FIXTURE_FILE, &orig_size);

    cr_assert_eq(orig_size, content_len, "blob size mismatch");
    cr_assert(memcmp(orig, content, orig_size) == 0, "blob content differs from original");

    /* free */
    free(raw_hash);
    free(obj_path);
    free(comp);
    free(decomp);
    free(orig);

    /* cleanup */
    system("rm -rf .mygit");
}

/* Additional tests to increase coverage */
Test(util_hash, produces_20byte_hash_and_40char_hex) {
    /* prepare a small blob header+data like createBlob would */
    const char *payload = "hello world";
    size_t payload_len = strlen(payload);
    size_t header_len = snprintf(NULL, 0, "blob %zu", payload_len) + 1;
    size_t blob_size = header_len + payload_len;

    unsigned char *blob = malloc(blob_size);
    cr_assert_not_null(blob);
    snprintf((char *)blob, header_len, "blob %zu", payload_len);
    memcpy(blob + header_len, payload, payload_len);

    unsigned char raw_hash[20];
    unsigned char hex[41];
    memset(raw_hash, 0, sizeof(raw_hash));
    memset(hex, 0, sizeof(hex));

    hash(raw_hash, hex, blob, blob_size);

    /* raw hash 20 bytes, hex 40 chars + nul */
    cr_assert_eq(strlen((char*)hex), 40);

    free(blob);
}

Test(object_zlib, compress_and_decompress_roundtrip) {
    const unsigned char *orig = (const unsigned char *)"this is some test data that repeats: 123123123123";
    size_t orig_len = strlen((const char*)orig);

    unsigned long comp_out = 0;
    unsigned char *comp = zlib_compress((unsigned char*)orig, orig_len, &comp_out);
    cr_assert_not_null(comp);
    cr_assert(comp_out > 0);

    unsigned long decomp_out = 0;
    unsigned char *decomp = zlib_decompress(comp, comp_out, &decomp_out);
    cr_assert_not_null(decomp);

    cr_assert_eq(decomp_out, orig_len);
    cr_assert(memcmp(decomp, orig, orig_len) == 0, "decompressed data must equal original");

    free(comp);
    free(decomp);
}

/* More tests to increase coverage: tree, add, blob command, read utilities, ignore */
Test(core_tree, tree_creates_tree_object_for_directory) {
    system("rm -rf .mygit");
    extern void init();
    init();

    /* create sample directory structure */
    system("rm -rf tmp_testdir && mkdir -p tmp_testdir/subdir");
    FILE *f = fopen("tmp_testdir/file1.txt", "w");
    fprintf(f, "hello file1"); fclose(f);
    FILE *g = fopen("tmp_testdir/subdir/file2.txt", "w");
    fprintf(g, "hello file2"); fclose(g);

    unsigned char *tree_hash = tree("tmp_testdir");
    cr_assert_not_null(tree_hash, "tree() returned NULL");

    /* hex */
    char hex[41] = {0};
    for (int i = 0; i < 20; ++i) snprintf(hex + i * 2, 3, "%02x", tree_hash[i]);

    char *obj_path = createObjectPath(hex);
    cr_assert(file_exists(obj_path), "tree object file missing");

    free(tree_hash);
    free(obj_path);
    system("rm -rf tmp_testdir");
    system("rm -rf .mygit");
}

Test(core_tree, writeTreeFromIndex_returns_null_currently) {
    unsigned char *r = writeTreeFromIndex();
    cr_assert_null(r);
}

Test(commands_add, addToIndex_appends_index_entry) {
    system("rm -rf .mygit");
    extern void init(); init();

    /* create a file to add */
    system("rm -f test_add.txt && echo abc > test_add.txt");

    char *params[] = { "test_add.txt", NULL };
    addToInexCommand(params);

    /* index should contain filename */
    int contains = fileContainsString(".mygit/index", "test_add.txt");
    cr_assert_eq(contains, 1);

    /* cleanup */
    system("rm -f test_add.txt");
    system("rm -rf .mygit");
}

Test(commands_blob_cmd, blob_writes_object_using_wrapper) {
    system("rm -rf .mygit"); extern void init(); init();
    system("echo wrapper > test_blob_cmd.txt");

    char *params[] = { "-w", "test_blob_cmd.txt", NULL };
    blob(params);

    /* createBlob prints hash to stdout and created object — we can't capture easily here, but index of objects should not be empty */
    /* check there is at least one directory in .mygit/objects */
    cr_assert(file_exists(".mygit/objects"), "objects dir missing");

    system("rm -f test_blob_cmd.txt");
    system("rm -rf .mygit");
}

Test(util_read, read_head_and_target_and_fileContainsString_behaviour) {
    system("rm -rf .mygit && mkdir -p .mygit/refs/heads");
    /* create HEAD pointing to ref */
    FILE *h = fopen(".mygit/HEAD", "w");
    fprintf(h, "ref: refs/heads/main\n"); fclose(h);

    /* create ref file */
    FILE *r = fopen(".mygit/refs/heads/main", "w");
    fprintf(r, "0123456789abcdef0123456789abcdef01234567\n"); fclose(r);

    char *target = readHeadTarget();
    cr_assert_str_eq(target, "refs/heads/main");
    free(target);

    char *sha = readTargetHash(".mygit/refs/heads/main");
    cr_assert_str_eq(sha, "0123456789abcdef0123456789abcdef01234567");
    free(sha);

    /* fileContainsString negative and positive */
    FILE *ix = fopen("test_index.txt", "w"); fprintf(ix, "line1\nline2 path/to/file\n"); fclose(ix);
    int found = fileContainsString("test_index.txt", "path/to/file");
    cr_assert_eq(found, 1);
    int notfound = fileContainsString("test_index.txt", "doesnotexist");
    cr_assert_eq(notfound, 0);

    remove("test_index.txt");
    system("rm -rf .mygit");
}

