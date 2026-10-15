#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gost_3411_2012_calc.h"

#define FILE_BUFFER_SIZE 4096
#define DEFAULT_HASH_SIZE 512

TGOSTHashContext *CTX;

static void HashPrint(TGOSTHashContext *CTX)
{
    printf("%d bit hash sum: \n", CTX->hash_size);
    int i;
    if (CTX->hash_size == 256)
        for(i = 32; i < 64; i++)
            printf("%02x", CTX->hash[i]);
    else
        for(i = 0; i < 64; i++)
            printf("%02x", CTX->hash[i]);
    printf("\n");
}

char * GetHashString(const char *str, int hash_size)
{
    uint8_t *buffer;
    buffer = malloc(strlen(str));
    memcpy(buffer, str, strlen(str));
    GOSTHashInit(CTX, hash_size);
    GOSTHashUpdate(CTX, buffer, strlen(str));
    GOSTHashFinal(CTX);
    printf("GOST 34.11-2012 \"Stribog\"\nString: %s\n", str);
    HashPrint(CTX);
    return CTX;
}