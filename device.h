#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "list.h"

struct node_t;
struct hash_alg_t; 

typedef struct device_t{
    char hash[512];
    char vid[32];
    char pid[32];           
    char manufacturer[128];
    char product[128];
    char serial[128];
}device;

int get_usb_device_info(const char *sys_path, device *dev);

node * init_devices_list();

int devices_count(void);

void clear_devices_list(node * devices);

void hashing_gevices_id(node * devices, struct hash_alg_t * hash_alg);