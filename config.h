#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "list.h"
#include "./crypto/crypt.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h> 
#include <pwd.h>
#include "device.h"
#include "list.h"

struct user_device{
    unsigned user_id;
    node * devices;
};


void write_config(node * devices);

char * get_user_config(unsigned user_id);

node * read_config_to_list(const char *usr_config_path);