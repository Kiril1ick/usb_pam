#include <stdio.h>
#include "device.h"
#include "list.h"
#include "config.h"

int main() {
    printf("Сканирование USB-устройств в связный список...\n");
    
    struct node_t *my_list = init_devices_list();
    
    if (my_list == NULL) {
        printf("Валидных USB-устройств не найдено или ошибка памяти.\n");
        return 0;
    }
    printf("Устройства успешно просканированы. Запись конфигурации...\n");
    write_config(my_list);
    clear_devices_list(my_list);
    printf("Память очищена. Программа завершена.\n");
    
    return 0;
}