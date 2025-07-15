#include "hash.h"
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>

void hash(unsigned char* hash, unsigned char* hash_hex, unsigned char* blob, size_t blob_size)
{
    SHA1(blob, blob_size, hash);
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        snprintf((hash_hex) + i * 2, 3, "%02x", (hash)[i]);
    }

    hash_hex[SHA_DIGEST_LENGTH * 2] = '\0';
}
