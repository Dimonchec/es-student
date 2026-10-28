cmake_minimum_required(VERSION 3.15)
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)
include(pico_sdk_import.cmake)
project(231_firmware_template)
pico_sdk_init()

# 1. Подключаем libs (лежит уровнем выше)
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

# 2. Предупреждения
add_compile_options(-Wall)

# 3. Только файлы проекта (без исходников библиотек!)
add_executable(${PROJECT_NAME}
    main.c
    api/api.c
    led/led.c	                         # <--- Указываем путь к папке api
    led-task/led-task.c               # <--- Указываем путь
    button-task/button-task.c         # <--- Указываем путь
    pi-task/pi-task.c                 # <--- Указываем путь
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/led
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task
    ${CMAKE_CURRENT_SOURCE_DIR}/button-task
    ${CMAKE_CURRENT_SOURCE_DIR}/pi-task
)

# 4. Линкуем библиотеки
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    device
    logging
    profiling
    systime
    stdio_text_protocol
)

# 5. Выключатель профилирования
target_compile_definitions(profiling PUBLIC PROFILING_ENABLED=1)

target_compile_definitions(${PROJECT_NAME} PRIVATE DEVICE_BOARD="${PICO_BOARD}")

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})