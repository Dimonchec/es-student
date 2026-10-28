#pragma once
#include <stdint.h>

typedef struct {
    const char *project;
    const char *version;
    const char *repo;
    const char *board;
    char serial[17];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    const char *sdk_version;
} device_info_t;

// <--- ИЗМЕНЕНО: добавлены аргументы project, version, repo
void device_get_info(device_info_t *info, const char *project, const char *version, const char *repo);