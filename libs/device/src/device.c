#include "device/device.h"
#include "pico/version.h"
#include "pico/unique_id.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
#include <string.h>

// <--- ИЗМЕНЕНО: добавлены аргументы project, version, repo
void device_get_info(device_info_t *info, const char *project, const char *version, const char *repo)
{
    info->project = project;
    info->version = version;
    info->repo = repo;
    info->board = DEVICE_BOARD; // Это значение приходит из CMakeLists.txt библиотеки

    // Читаем серийный номер
    pico_get_unique_board_id_string(info->serial, sizeof(info->serial));

    // Читаем CHIP_ID
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;
    info->manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    info->part = (id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
    info->revision = (id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;
    
    info->sdk_version = PICO_SDK_VERSION_STRING;
}