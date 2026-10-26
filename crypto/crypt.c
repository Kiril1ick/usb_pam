#include "crypt.h"

//#define FILE_BUFFER_SIZE 4096
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
    TGOSTHashContext ctx;
    buffer = malloc(strlen(str));
    memcpy(buffer, str, strlen(str));
    GOSTHashInit(&ctx, hash_size);
    GOSTHashUpdate(&ctx, buffer, strlen(str));
    GOSTHashFinal(&ctx);
    printf("GOST 34.11-2012 \"Stribog\"\nString: %s\n", str);
    HashPrint(&ctx);
    free(buffer);
    int bytes_to_print = (hash_size == 256) ? 32 : 64;
    int start_index = (hash_size == 256) ? 32 : 0;

    // Выделяем память в куче под строку: по 2 символа на байт + 1 байт под ноль-терминатор '\0'
    char *hex_result = malloc((bytes_to_print * 2) + 1);
    if (hex_result == NULL) return NULL;

    // Заполняем текстовый буфер шестнадцатеричными символами
    for (int i = 0; i < bytes_to_print; i++) {
        sprintf(&hex_result[i * 2], "%02x", ctx.hash[start_index + i]);
    }
    hex_result[bytes_to_print * 2] = '\0'; // Гарантируем корректный конец строки Си

    // 3. Возвращаем живой указатель на динамическую память, которая не исчезнет при выходе
    return hex_result;
}



char * get_hash(char * data, hash_alg * hash_alg){
    /*char * hash = NULL;
    if(hash_alg.halg == 1){
        if(hash_alg.hsize == 512 || hash_alg.hsize == 256){
            hash = GetHashString(devices->device->hash, hash_alg.hsize);
        }
        else
            return NULL;
        
    }
    return hash;*/
    if(data != NULL && hash_alg != NULL){
        //TODO Устранить утечку памяти. Из функции GetHashString возвращается указатель на строку, а CTX не освобождается
        char * hash = GetHashString(data, hash_alg->hsize);

    }
    else
        return NULL;
};