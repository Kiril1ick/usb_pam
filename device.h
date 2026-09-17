#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "list.h"

typedef struct device_t{
    char path[256];
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