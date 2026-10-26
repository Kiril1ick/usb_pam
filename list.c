#include "list.h"
#include "device.h" 

node * init_list(){
    node * new_node = malloc(sizeof(node));
    if(new_node == NULL)
        return NULL;
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->device = NULL;
    return new_node;
}

void insert_device(node ** list, device_t * device){
    if (list == NULL || device == NULL) {
        return; 
    }
    node * new_head = init_list();
    if(new_head == NULL){
        fprintf(stderr, "Неудалось добавить в список устройство с Vendor id: %s\n", device->pid);
        return;
    }
    new_head->device = device;
    new_head->next = (*list);
    new_head->prev = NULL;
    if ((*list) != NULL) {
        (*list)->prev = new_head;
    }
    (*list) = new_head;
}

void remove_node(node ** list, node * rnode) {
    if (list == NULL || *list == NULL || rnode == NULL) {
        return;
    }
    
    if (*list == rnode) {
        *list = rnode->next;
    }
    
    if (rnode->prev != NULL) {
        rnode->prev->next = rnode->next;
    }
    
    if (rnode->next != NULL) {
        rnode->next->prev = rnode->prev;
    }

    if (rnode->device != NULL) {
        free(rnode->device);
    }
    free(rnode);
}

void print_devices(node * list){
    node * tmp = list;
    while (tmp != NULL){
        if(tmp->device != NULL && tmp->device->hash != NULL)
            printf("%s\n", tmp->device->hash);
        tmp = tmp->next;
    }
    
}

node * search_in_list(node * list, device * sdev) {
    if (list == NULL || sdev == NULL) {
        return NULL;
    }

    node * tmp = list;
    while (tmp != NULL) {
        if (tmp->device != NULL && 
            strcmp(tmp->device->vid, sdev->vid) == 0 &&
            strcmp(tmp->device->pid, sdev->pid) == 0)
        {
            return tmp;
        }
        
        tmp = tmp->next;
    }
    
    return NULL;
}