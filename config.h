#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "list.h"


struct user_device{
    unsigned user_id;
    node * devices;
};


void write_config(node * devices);