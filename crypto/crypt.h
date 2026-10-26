#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "./stribog/gost_3411_2012_calc.h"
#include "../list.h"
#include "../device.h"


typedef enum {STRIBOG = 1, SHA = 2} hash_func;
typedef enum  {KEY512 = 512, KEY256 = 256} hash_size;

typedef struct hash_alg_t{
    hash_func halg;
    hash_size hsize;
}hash_alg;

char * get_hash(char * data, hash_alg * hash_alg);