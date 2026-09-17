#pragma once

#include <stdio.h>

// Опережающее объявление структуры устройства
typedef struct device_t device_t; 

typedef struct node_t {
    struct node_t * next;
    struct node_t * prev;
    device_t * device;
} node;

node * init_list(void);
void insert_device(node ** list, device_t * device);
void print_devices(node * list);
node * search_in_list(node * list, device_t * sdev);