#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h> 
#include "device.h"
#include "config.h"
#include "list.h"

const char *main_config_path = "/etc/usb_pam.cfg";

int create_cfg_dir(char * dir_name) {
    
    int check = mkdir(dir_name, 0666);
    if (check == 0) return 1;
    
    if (check == -1 && errno == EEXIST) return 1;
    
    return 0;
}

void write_config(node * devices) {
    // Защита от передачи пустого списка
    if (devices == NULL) {
        return; 
    }

    struct user_device ud;
    int user_id = getuid();
    ud.user_id = user_id;
    ud.devices = devices;
    char buff[1024] = {0};

    char * home_dir = getenv("HOME");
    if (home_dir == NULL) {
        fprintf(stderr, "ERROR: Переменная окружения HOME не найдена\n");
        return;
    }

    strcat(buff, home_dir);
    strcat(buff, "/.usb_pam");
    printf("HOME: %s\n", buff);
    
    // Создаем директорию. Теперь повторный запуск полностью безопасен.
    int check = create_cfg_dir(buff);
    if (!check) {
        fprintf(stderr, "ERROR: Не удалось создать директорию конфигурации\n");
        return; // Мягкий выход вместо жесткого abort()
    }

    // Открываем главный конфиг
    FILE * fd = fopen(main_config_path, "a");
    if (fd == NULL) {
        // ИСПРАВЛЕНО: Защита от падения, если забыли запустить через sudo
        perror("ERROR: Не удалось открыть /etc/usb_pam.cfg (требуются права root/sudo)");
        return; 
    }

    strcat(buff, "/usr.config");
    fprintf(fd, "%u:%s\n", ud.user_id, buff);
    fclose(fd);

    // Открываем персональный конфиг пользователя
    fd = fopen(buff, "w");
    if (fd == NULL) {
        printf("ERROR: Can`t open usr.cfg: %s\n", buff);
        return; 
    }

    buff[0] = 0;
    node * tmp = ud.devices;
    while (tmp != NULL) {
        if (tmp->device != NULL) {
            strcat(buff, tmp->device->path);
            strcat(buff, ":");
            strcat(buff, tmp->device->vid);
            strcat(buff, ":");
            strcat(buff, tmp->device->pid);
            strcat(buff, ":");
            strcat(buff, tmp->device->manufacturer);
            strcat(buff, ":");
            strcat(buff, tmp->device->product);
            strcat(buff, ":");
            strcat(buff, tmp->device->serial);
            strcat(buff, "\n");
            
            fprintf(fd, "%s", buff);
            buff[0] = 0; // Очищаем буфер для следующего устройства
        }
        tmp = tmp->next;
    }
    fclose(fd);
}