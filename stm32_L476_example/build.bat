@echo off

:: Set default for BUILD_OPEE_TESTS to OFF
set BUILD_OPEE_TESTS=OFF

:: Check if the user provided the BUILD_OPEE_TESTS argument
if "%1"=="--tests" (
    set BUILD_OPEE_TESTS=ON
)

:: Display the chosen option
echo Building with tests: %BUILD_OPEE_TESTS%

:: Run the cmake command to configure the project
cmake -Bbuild -G Ninja -DCMAKE_TOOLCHAIN_FILE=D:\development\git\OPEEngine\stm32_L476_example\gcc-arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=true -DBUILD_OPEE_TESTS=%BUILD_OPEE_TESTS%
