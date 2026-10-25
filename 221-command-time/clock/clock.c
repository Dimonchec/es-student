#include "clock.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

// Вспомогательная функция для печати строки таблицы
static void row(const char *name, uint32_t set_khz, uint32_t measured_khz)
{
    printf("%-8s %9u %12u\n", name, (unsigned)set_khz, (unsigned)measured_khz);
}

void clk_info(void)
{
    // Шапка таблицы
    printf("%-8s %9s %12s\n", "signal", "set_khz", "measured_khz");

    // Настроенные частоты (clock_get_hz возвращает Гц, делим на 1000)
    // Измеренные частоты (frequency_count_khz возвращает кГц)
    
    row("clk_ref", clock_get_hz(clk_ref) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_REF));
    row("clk_sys", clock_get_hz(clk_sys) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS));
    row("clk_peri", clock_get_hz(clk_peri) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI));
    row("clk_usb", clock_get_hz(clk_usb) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB));
    row("clk_adc", clock_get_hz(clk_adc) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC));

    // Для ROSC нет настроенной частоты, только измеренная.
    // Вместо числа печатаем строку "-". 
    // Используем %s для строки, а для числа оставляем %u.
    printf("%-8s %9s %12u\n", "rosc", "-", (unsigned)frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC));
}