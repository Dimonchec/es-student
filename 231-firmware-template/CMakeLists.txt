cmake_minimum_required(VERSION 3.15)
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)
include(pico_sdk_import.cmake)
project(231_firmware_template)
pico_sdk_init()

# libs лежит вне папки проекта
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

add_compile_options(-Wall)

# 1. Платформа (работа с SDK)
add_library(platform STATIC
    platform/platform.c
)
target_include_directories(platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(platform PRIVATE pico_stdlib)

# 2. Логика (без SDK)
add_library(logic STATIC
    logic/api/api.c
    logic/api/api-callbacks.c
    logic/led-task/led-task.c
    logic/button-task/button-task.c
    logic/pi-task/pi-task.c
)
target_include_directories(logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logic ${CMAKE_CURRENT_SOURCE_DIR})

# Подключаем протокол как PUBLIC, остальные библиотеки как PRIVATE
target_link_libraries(logic 
    PUBLIC stdio_text_protocol 
    PRIVATE systime logging profiling device
)

# 3. Главный исполняемый файл
add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    platform 
    logic
    profiling   # <--- ДОБАВИТЬ
    logging     # <--- ДОБАВИТЬ (для LOG_INF)
    systime     # <--- ДОБАВИТЬ (если main.c использует systime_us)
)

target_compile_definitions(profiling PUBLIC PROFILING_ENABLED=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE DEVICE_BOARD="${PICO_BOARD}")

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})