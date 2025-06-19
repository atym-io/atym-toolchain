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

# Specify paths to ocre_api
find_library(OCRE_API_LIB ocre_api
    PATHS /workspaces/getting-started-internal/atym-sdk/build
    NO_DEFAULT_PATH
)
find_path(OCRE_API_INCLUDE ocre_api.h
    PATHS /workspaces/getting-started-internal/atym-sdk
    NO_DEFAULT_PATH
)

if (NOT OCRE_API_LIB OR NOT OCRE_API_INCLUDE)
    message(FATAL_ERROR "ocre_api library or headers not found. Please ensure ocre_api is built.")
endif()

add_executable(${APPNAME}.wasm main.c)
target_include_directories(${APPNAME}.wasm PRIVATE ${OCRE_API_INCLUDE})
target_link_libraries(${APPNAME}.wasm PRIVATE ${OCRE_API_LIB})