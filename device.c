#include "device.h"
#include "list.h"

char *base_path = "/sys/bus/usb/devices";

static void read_sysfs_attribute(const char *dev_path, const char *attr_name, char *output, size_t max_len) {
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s", dev_path, attr_name);
    
    FILE *fp = fopen(file_path, "r");
    if (fp) {
        if (fgets(output, max_len, fp) != NULL) {
            output[strcspn(output, "\n")] = '\0';
        } else {
            strncpy(output, "-", max_len);
        }
        fclose(fp);
    } else {
        strncpy(output, "-", max_len);
    }
}


int get_usb_device_info(const char *sys_path, device *dev) {
    if (sys_path == NULL || dev == NULL) {
        return 0;
    }

    strncpy(dev->path, sys_path, sizeof(dev->path) - 1);
    dev->path[sizeof(dev->path) - 1] = '\0';

    read_sysfs_attribute(sys_path, "idVendor", dev->vid, sizeof(dev->vid));
    read_sysfs_attribute(sys_path, "idProduct", dev->pid, sizeof(dev->pid));
    read_sysfs_attribute(sys_path, "manufacturer", dev->manufacturer, sizeof(dev->manufacturer));
    read_sysfs_attribute(sys_path, "product", dev->product, sizeof(dev->product));
    read_sysfs_attribute(sys_path, "serial", dev->serial, sizeof(dev->serial));

    if (strcmp(dev->vid, "-") == 0) {
        return 0; 
    }

    return 1;
}

node * init_devices_list() {
    DIR *dir = opendir(base_path);
    if (dir == NULL) {
        perror("Не удалось открыть /sys/bus/usb/devices");
        return NULL;
    }

    node *devices_list = NULL;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strchr(entry->d_name, ':') != NULL || entry->d_name[0] == '.') {
            continue;
        }

        device *dev = malloc(sizeof(device));
        if (dev == NULL) {
            continue;
        }

        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry->d_name);

        if (get_usb_device_info(full_path, dev)) {
            node * old_val = search_in_list(devices_list, dev);
            if(old_val != NULL)
                continue;
            insert_device(&devices_list, dev);
        } else {

            free(dev);
        }
    }

    closedir(dir);
    return devices_list;
}

void clear_devices_list(node * devices) {
    node * current = devices;
    while (current != NULL) {
        node * next_node = current->next;
        
        if (current->device != NULL) {
            free(current->device);
        }
        
        free(current);
        current = next_node;
    }
}