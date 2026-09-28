#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h> 
#include "device.h"
#include "config.h"
#include "list.h"

const char *main_config_path = "/etc/usb_pam.cfg";
static const char *tmp_config_path = "/etc/usb_pam.tmp";

int create_cfg_dir(char * dir_name) {
    
    int check = mkdir(dir_name, 0666);
    if (check == 0) return 1;
    
    if (check == -1 && errno == EEXIST) return 1;
    
    return 0;
}


void write_config(node * devices) {
    if (devices == NULL) return; 

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
    
    int check = create_cfg_dir(buff);
    if (!check) {
        fprintf(stderr, "ERROR: Не удалось создать директорию конфигурации\n");
        return;
    }
    strcat(buff, "/usr.config"); // Теперь buff содержит путь к usr.config

    // --- НАЧАЛО ПРОВЕРКИ И ОБНОВЛЕНИЯ ГЛАВНОГО КОНФИГА ---
    
    // Проверяем, существует ли пользователь в базе данных
    char *existing_path = get_user_config(ud.user_id);

    if (existing_path == NULL) {
        // Сценарий 1: Пользователя нет в базе. Просто дописываем его в конец ("a")
        FILE *fd = fopen(main_config_path, "a");
        if (fd == NULL) {
            perror("ERROR: Не удалось открыть /etc/usb_pam.cfg для записи (нужен sudo)");
            return;
        }
        fprintf(fd, "%u:%s\n", ud.user_id, buff);
        fclose(fd);
    } else {
        // Сценарий 2: Пользователь уже есть. Освобождаем память от get_user_config
        free(existing_path);

        // Перезаписываем файл, исключая старую строку этого пользователя
        FILE *src = fopen(main_config_path, "r");
        FILE *dst = fopen(tmp_config_path, "w");
        
        if (src == NULL || dst == NULL) {
            perror("ERROR: Не удалось открыть файлы для обновления конфигурации");
            if (src) fclose(src);
            if (dst) fclose(dst);
            return;
        }

        char line[512];
        // Копируем всех остальных пользователей во временный файл
        while (fgets(line, sizeof(line), src) != NULL) {
            unsigned parsed_uid = 0;
            if (sscanf(line, "%u:", &parsed_uid) == 1) {
                if (parsed_uid != ud.user_id) {
                    fputs(line, dst); // Пишем только чужие строки
                }
            }
        }

        // В конце временного файла пишем обновленную строку нашего пользователя
        fprintf(dst, "%u:%s\n", ud.user_id, buff);

        fclose(src);
        fclose(dst);

        // Заменяем старый файл новым атомарно
        if (rename(tmp_config_path, main_config_path) != 0) {
            perror("ERROR: Не удалось обновить главный файл конфигурации");
            unlink(tmp_config_path); // Удаляем временный файл при ошибке
            return;
        }
    }
    // --- КОНЕЦ ПРОВЕРКИ И ОБНОВЛЕНИЯ ГЛАВНОГО КОНФИГА ---

    // 3. Запись персонального конфига пользователя (без изменений)
    FILE *fd = fopen(buff, "w");
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
            buff[0] = 0; 
        }
        tmp = tmp->next;
    }
    fclose(fd);
}

char * get_user_config(unsigned user_id) {
    FILE * fd = fopen(main_config_path, "r");
    if (fd == NULL) {
        return NULL;
    }
    
    char line[512];
    char *result_path = NULL;

    while (fgets(line, sizeof(line), fd) != NULL) {
        unsigned parsed_uid = 0;
        char tmp_path[256] = {0};
        if (sscanf(line, "%u:%255s", &parsed_uid, tmp_path) == 2) {
            if (parsed_uid == user_id) {
                if (result_path != NULL) {
                    free(result_path);
                }
                result_path = strdup(tmp_path); 
            }
        }
    }
    
    fclose(fd);
    return result_path;
}

node * read_config_to_list(const char *usr_config_path) {
    if (usr_config_path == NULL) {
        return NULL;
    }

    FILE *fd = fopen(usr_config_path, "r");
    if (fd == NULL) {
        perror("ERROR: Не удалось открыть персональный файл конфигурации для чтения");
        return NULL;
    }

    node *allowed_devices_list = NULL; // Изначально список пуст
    char line[1024];

    // Построчно читаем usr.config
    while (fgets(line, sizeof(line), fd) != NULL) {
        // Удаляем перенос строки в конце, если он есть
        line[strcspn(line, "\n")] = '\0';

        // Пропускаем пустые строки
        if (strlen(line) == 0) {
            continue;
        }

        // Выделяем память под структуру одного устройства (используем ваше имя типа 'device')
        device *dev = malloc(sizeof(device));
        if (dev == NULL) {
            perror("ERROR: Не хватило памяти для устройства");
            continue; 
        }
        memset(dev, 0, sizeof(device));

        // Разбираем строку формата PATH:VID:PID:MANUFACTURER:PRODUCT:SERIAL
        // Спецификатор %[^:] означает "читать все символы, пока не встретится двоеточие"
        int parsed = sscanf(line, "%255[^:]:%31[^:]:%31[^:]:%127[^:]:%127[^:]:%127[^\n]",
                            dev->path, dev->vid, dev->pid, 
                            dev->manufacturer, dev->product, dev->serial);

        // Устройство считается валидным, если прочитались как минимум PATH, VID и PID (3 поля)
        if (parsed >= 3) {
            // Вставляем устройство в двусвязный список
            insert_device(&allowed_devices_list, dev);
        } else {
            // Если строка оказалась битой или неполной — освобождаем память
            free(dev); 
        }
    }

    fclose(fd);
    return allowed_devices_list; // Возвращаем получившийся белый список устройств
}