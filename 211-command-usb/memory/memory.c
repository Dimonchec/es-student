#include "memory.h"
#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"
#include <stdlib.h> // Для malloc и free
#include "command.h"
#include "device.h"

// Объявляем символы линкера
extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

uint32_t data_variable = 100;
uint32_t bss_variable;

// Объявление main из другого файла (как сказано в задании)
extern int main(void);

void fw_info(void)
{
    // 1. Увеличиваем значения
    data_variable++;
    bss_variable++;

    // 2. Адрес main (сбрасываем признак Thumb)
    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    
    // 3. Локальные переменные (стек и куча)
    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    // 4. Печать шапки
    printf("%-20s %-10s %s\n", "object", "address", "value");

    // 5. Печать данных
    // main: адрес с признаком Thumb и машинное слово (2 байта) по четному адресу
    printf("%-20s 0x%08x 0x%04x\n", "main", (unsigned)main, *main_code);
    
    // fw_info: свой адрес (с признаком Thumb)
    printf("%-20s 0x%08x\n", "fw_info", (unsigned)fw_info);
    
    // commands: адрес массива
    printf("%-20s 0x%08x\n", "commands", (unsigned)commands);

    // Обход массива команд
    for (uint i = 0; i < command_count; i++)
    {
        printf("- %-18s 0x%08x\n", commands[i].name, (unsigned)commands[i].handler);
    }

    // Константы из device.h (нужен оператор &)
    printf("%-20s 0x%08x %s\n", "DEVICE_PROJECT", (unsigned)&DEVICE_PROJECT, DEVICE_PROJECT);
    printf("%-20s 0x%08x %s\n", "DEVICE_BOARD", (unsigned)&DEVICE_BOARD, DEVICE_BOARD);
    // Можно добавить DEVICE_NAME и FIRMWARE_VERSION по аналогии

    // Переменные в RAM
    printf("%-20s 0x%08x %u\n", "data_variable", (unsigned)&data_variable, data_variable);
    printf("%-20s 0x%08x %u\n", "bss_variable", (unsigned)&bss_variable, bss_variable);
    printf("%-20s 0x%08x %u\n", "stack_variable", (unsigned)&stack_variable, stack_variable);
    
    // Куча: печатаем адрес блока, а не указателя!
    if (heap_variable != NULL)
    {
        printf("%-20s 0x%08x %u\n", "heap_variable", (unsigned)heap_variable, *heap_variable);
    }

    // 6. Возвращаем память
    free(heap_variable);
}

// Вспомогательная функция для печати строки таблицы
static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    // Шапка таблицы
    printf("%-10s %-10s %-10s %8s\n", "area", "start", "end", "size");

    // Базовые области (из документации и макросов SDK)
    row("flash", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("sram", SRAM_BASE, SRAM_BASE + 264 * 1024);
    row("rom", ROM_BASE, ROM_BASE + 16 * 1024);

    // Области, задаваемые символами линкера
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    row("free", (uintptr_t)&__flash_binary_end, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);
    
    // .data занимает 2 места: во флеше (хранится) и в ОЗУ (работает)
       // Стало:
    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext + ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__));
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    // Итоги (подсчет)
    unsigned flash_image = ((uintptr_t)&__flash_binary_end - (uintptr_t)&__flash_binary_start);
    unsigned flash_free = (PICO_FLASH_SIZE_BYTES) - flash_image;
    unsigned ram_used = ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__) + ((uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__);
    unsigned ram_free = ((uintptr_t)&__StackTop - (uintptr_t)&__HeapLimit) + ((uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__);

    printf("\ntotal\n");
    printf("  flash image   %u\n", flash_image);
    printf("  flash free    %u of %u\n", flash_free, (unsigned)PICO_FLASH_SIZE_BYTES);
    printf("  ram used      %u\n", ram_used);
    printf("  ram free      %u for heap and %u for stack\n", 
           (unsigned)((uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__), 
           (unsigned)((uintptr_t)&__StackTop - (uintptr_t)&__StackBottom));
}