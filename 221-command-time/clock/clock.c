#include "clock.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "log.h" 

// Константа пониженной частоты
const uint32_t CLK_SYS_LOW_KHZ = 62500;

// Статическая функция установки частоты
static void clk_sys_set(uint32_t khz)
{
    if (set_sys_clock_khz(khz, false))
    {
        LOG_INF("clk_sys %u kHz\n", (unsigned)khz);
    }
    else
    {
        LOG_ERR("clk_sys %u kHz is not set\n", (unsigned)khz);
    }
}

// Функции для смены частоты
void clk_sys_low(void)
{
    clk_sys_set(CLK_SYS_LOW_KHZ);
}

void clk_sys_default(void)
{
    clk_sys_set(SYS_CLK_KHZ); // SYS_CLK_KHZ - макрос из SDK (125000)
}

// ВАЖНО: Добавьте этот комментарий над функцией clk_sys_set (как требует задание)
// set_sys_clock_khz() вызывает set_sys_clock_pll(), а та переводит clk_peri
// на PLL для USB, 48 МГц. Так настроено по умолчанию:
// PICO_CLOCK_ADJUST_PERI_CLOCK_WITH_SYS_CLOCK равен 0.

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
void uptime(void)
{
    // time_us_64() возвращает uint64_t (микросекунды).
    // Делим на 1000, чтобы получить миллисекунды.
    printf("uptime: %llu ms\n", time_us_64() / 1000);
}