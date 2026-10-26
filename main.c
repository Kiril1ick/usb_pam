#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "device.h"
#include "list.h"
#include "config.h"
#include "./crypto/crypt.h"

int main() {
    unsigned uid = getuid();
    printf("--- Тестирование USB PAM-модуля для UID %u ---\n", uid);

    // 1. Сканируем текущую физическую шину компьютера
    printf("Сканирование физических USB-устройств...\n");
    struct node_t *my_cur_list = init_devices_list();
    if (my_cur_list == NULL) {
        printf("Валидных физических USB-устройств не найдено или ошибка памяти.\n");
        return 0;
    }

    printf("\n=== ТЕКУЩИЕ ПОДКЛЮЧЕННЫЕ УСТРОЙСТВА (Физические) ===\n");
    hash_alg hash_alg;
    hash_alg.halg = STRIBOG;
    hash_alg.hsize = KEY512;
    hashing_gevices_id(my_cur_list, &hash_alg);
    print_devices(my_cur_list);
    write_config(my_cur_list);
    clear_devices_list(my_cur_list);
}