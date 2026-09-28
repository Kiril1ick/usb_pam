#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "device.h"
#include "list.h"
#include "config.h"

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
    print_devices(my_cur_list);

    // 2. Ищем путь к персональному файлу конфигурации в /etc/usb_pam.cfg
    char *cfg_path = get_user_config(uid);
    if (cfg_path == NULL) {
        printf("\nКонфигурация для пользователя не найдена в базе данных.\n");
        printf("Сначала запустите сканирование с активной функцией write_config().\n");
        clear_devices_list(my_cur_list);
        return 0;
    }
    printf("\nНайден путь к зарегистрированному белому списку: %s\n", cfg_path);

    // 3. Восстанавливаем белый список устройств из файла конфигурации
    node * cfg_list = read_config_to_list(cfg_path);
    free(cfg_path); // Освобождаем динамическую строку, возвращенную из get_user_config

    printf("\n=== УСТРОЙСТВА ИЗ БЕЛОГО СПИСКА (Зарегистрированные) ===\n");
    if (cfg_list != NULL) {
        print_devices(cfg_list);
    } else {
        printf("(Белый список пуст или файл конфигурации еще не был создан)\n");
    }

    // 4. Логика сверки (опционально, для наглядности теста)
    printf("\nПроверка авторизации...\n");
    int access_granted = 0;
    node *tmp = my_cur_list;
    while (tmp != NULL) {
        if (tmp->device != NULL && search_in_list(cfg_list, tmp->device) != NULL) {
            access_granted = 1;
            break;
        }
        tmp = tmp->next;
    }
    
    if (access_granted) {
        printf(">>> ДОСТУП РАЗРЕШЕН: Найдено доверенное USB-устройство <<<\n");
    } else {
        printf(">>> ДОСТУП ЗАПРЕЩЕН: Доверенные устройства не обнаружены <<<\n");
    }

    // 5. Корректно очищаем всю выделенную память для обоих списков
    clear_devices_list(my_cur_list);
    if (cfg_list != NULL) {
        clear_devices_list(cfg_list);
    }

    printf("\nПамять очищена. Программа завершена.\n");
    return 0;
}