cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_TOOLCHAIN_FILE /opt/wasi-sdk/share/cmake/wasi-sdk.cmake)
set(APPNAME sensor-imu)
project(${APPNAME} LANGUAGES C)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--strip-all -Wl,--allow-undefined")
add_compile_options(
    -O3
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unknown-attributes
)

# Add the atym-sdk as a subdirectory and build it
add_subdirectory($ENV{ATYM_SDK_PATH} atym-sdk-build)

add_executable(${APPNAME}.wasm main.c)
target_include_directories(${APPNAME}.wasm PRIVATE ocre_api)
target_link_libraries(${APPNAME}.wasm PRIVATE ocre_api)