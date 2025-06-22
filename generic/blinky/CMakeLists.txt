cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_TOOLCHAIN_FILE /opt/wasi-sdk/share/cmake/wasi-sdk.cmake)
set(APPNAME blinky)
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
set(ATYM_SDK_PATH "../atym-sdk")
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ATYM_SDK_PATH}/CMakeLists.txt")
    message(FATAL_ERROR "atym-sdk not found at ${ATYM_SDK_PATH}. Please ensure the atym-sdk directory exists and contains CMakeLists.txt")
endif()

add_subdirectory(${ATYM_SDK_PATH} atym-sdk-build)

add_executable(${APPNAME}.wasm main.c)
target_include_directories(${APPNAME}.wasm PRIVATE ocre_api)
target_link_libraries(${APPNAME}.wasm PRIVATE ocre_api)