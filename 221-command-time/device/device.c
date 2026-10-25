#include "device.h"
#include "pico/version.h"
#include <stdio.h>
#include <stddef.h> // Для offsetof
#include <string.h>
#include "pico/unique_id.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"

// Заводим и заполняем переменную
struct info_t device_card = {
    .revision = 2,
    .version = 0x00010000,
    .name = "es-cmd-usb"
};

void device_info(void)
{
    // 1. Читаем серийный номер платы
    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));

    // 2. Читаем регистр CHIP_ID по адресу и разбираем три поля
    // Указываем компилятору, что это регистр (volatile), а не просто память
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;

    // Выделяем производителя (manufacturer)
    uint32_t manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    
    // Выделяем номер изделия (part)
    uint32_t part = (id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
    
    // Выделяем ревизию (revision)
    uint32_t revision = (id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;

    // 3. Печатаем пять строк паспорта
    // ВАЖНО: здесь используем обычный printf, а не LOG_..., так как паспорт 
    // не должен исчезать при понижении уровня логирования
    printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", DEVICE_BOARD);
    printf("serial: %s\n", board_id);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n",
           manufacturer, part, revision);
    printf("pico-sdk: %s\n", PICO_SDK_VERSION_STRING);
}

void dev_info(void)
{
    // Шапка таблицы
    printf("%-15s %-10s %-5s %-6s %s\n", "struct", "address", "size", "offset", "value");

    // Строка самой структуры
    printf("%-15s 0x%08x %-5u\n", "device_card", (unsigned)&device_card, (unsigned)sizeof(device_card));

    // Поля структуры (для revision)
    printf("- %-13s 0x%08x %-5u %-6u %u\n", 
           "revision", 
           (unsigned)&device_card.revision, 
           (unsigned)sizeof(device_card.revision), 
           (unsigned)offsetof(struct info_t, revision), 
           device_card.revision);

    // Поля структуры (для version)
    printf("- %-13s 0x%08x %-5u %-6u 0x%08x\n", 
           "version", 
           (unsigned)&device_card.version, 
           (unsigned)sizeof(device_card.version), 
           (unsigned)offsetof(struct info_t, version), 
           device_card.version);

    // Поля структуры (для name) - здесь & не нужен, так как массив это уже адрес
    printf("- %-13s 0x%08x %-5u %-6u %s\n", 
           "name", 
           (unsigned)device_card.name, 
           (unsigned)sizeof(device_card.name), 
           (unsigned)offsetof(struct info_t, name), 
           device_card.name);

    // Итог
    unsigned fields = sizeof(device_card.version) + sizeof(device_card.name) + sizeof(device_card.revision);
    printf("fields %u, sizeof %u, padding %u\n", fields, (unsigned)sizeof(device_card), (unsigned)(sizeof(device_card) - fields));
}