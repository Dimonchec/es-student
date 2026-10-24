#include "memory.h"
#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"

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
    row("data_flash", (uintptr_t)&__etext, (uintptr_t)&__etext + ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__));
    row("data_ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    
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