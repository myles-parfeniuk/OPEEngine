
cmake_minimum_required(VERSION 3.12)

option(BUILD_OPEE_TESTS "Enable testing with Unity" OFF)

#### SETUP ####
project(stm32_L476_example)

set(MCU_FAMILY STM32L4xx)
set(MCU_MODEL STM32L476xx)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard)
    
set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/startup_stm32l476xx.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/STM32L476RGTx_FLASH.ld)
set(EXECUTABLE ${CMAKE_PROJECT_NAME})

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core/app)
#### SETUP DONE ####

#### INCLUDES ####
set(CUBEMX_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include)

set(FREERTOS_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
)

set(PROJECT_INCLUDE_DIRECTORIES
    ${PROJECT_DIR}
    )
#### INCLUDES END ####


#### SOURCES ####
file(GLOB_RECURSE PROJECT_SOURCES
${PROJECT_DIR}/*.cpp
${PROJECT_DIR}/*.c
)

file(GLOB_RECURSE PROJECT_SOURCES FOLLOW_SYMLINKS
${PROJECT_DIR}/*.cpp
${PROJECT_DIR}/*.c)

file(GLOB_RECURSE STM32CUBEMX_SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Source/*.c
)

file(GLOB_RECURSE FREERTOS_SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/*.c
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/*.c
)
#### SOURCES END ####

#### SOURCES EXCLUSIONS ####
file(GLOB_RECURSE EXCLUDE_STM32CUBEMX_TEMPLATE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Source/Templates/system_stm32l4xx.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/stm32l4xx_hal_timebase_rtc_alarm_template.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/stm32l4xx_hal_timebase_rtc_wakeup_template.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/stm32l4xx_hal_timebase_tim_template.c
${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/stm32l4xx_hal_msp_template.c

)

file(GLOB_RECURSE EXCLUDE_FREERTOS_TEMPLATE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_1.c
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_2.c
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_3.c
${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_5.c
)

list(REMOVE_ITEM STM32CUBEMX_SOURCES ${EXCLUDE_STM32CUBEMX_TEMPLATE_FILES})

list(REMOVE_ITEM FREERTOS_SOURCES ${EXCLUDE_FREERTOS_TEMPLATE_FILES})
#### SOURCES EXCLUSIONS END ####

#### ADD EXECUTABLE ####
add_executable(${EXECUTABLE}
    ${STM32CUBEMX_SOURCES} 
    ${FREERTOS_SOURCES}
    ${PROJECT_SOURCES}
    ${STARTUP_SCRIPT})
#### ADD EXECUTABLE END ####

### ADD INCLUDES TO EXECUTABLE ###
target_include_directories(${EXECUTABLE} PRIVATE
    ${CUBEMX_INCLUDE_DIRECTORIES}
    ${FREERTOS_INCLUDE_DIRECTORIES}
    ${PROJECT_INCLUDE_DIRECTORIES})
### ADD INCLUDES TO EXECUTABLE END ###

#### SUBDIRECTORIES ####
add_subdirectory(Core/vendor/OPEEngine)

if(BUILD_OPEE_TESTS)
    add_subdirectory(Core/test)
endif()
#### SUBDIRECTORIES END ####

#### LIBRARY LINKING ####
target_link_libraries(${EXECUTABLE} PRIVATE OPEEngine)
### LIBRARY LINKING END ####

### COMPILE DEFINITIONS ####
target_compile_definitions(${EXECUTABLE} PRIVATE
    ${MCU_MODEL}
    USE_HAL_DRIVER)
### COMPILE DEFINITIONS END ####

### COMPILER AND LINKER SETTINGS ####
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-O0 -g3 -ggdb>)
    #$<$<CONFIG:Release>:-O1 -g0>
    #$<$<CONFIG:Release>:-Og -g0>)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -Wl,--end-group
    -Wl,--print-memory-usage)

### COMPILER AND LINKER SETTINGS END ####

### CUSTOM COMPILATION SETTINGS AND COMMANDS ####

#disable warnings on freertos and cube files
set(FREERTOS_COMPILE_OPTIONS
-w
)

set(STM32CUBEMX_COMPILE_OPTIONS
-w
)

set_source_files_properties(${FREERTOS_SOURCES} PROPERTIES COMPILE_OPTIONS "${FREERTOS_COMPILE_OPTIONS}")

set_source_files_properties(${STM32CUBEMX_SOURCES} PROPERTIES COMPILE_OPTIONS "${STM32CUBEMX_COMPILE_OPTIONS}")

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}>
    ${EXECUTABLE}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}>
    ${EXECUTABLE}.bin)
### CUSTOM COMPILATION SETTINGS AND COMMANDS END ####
