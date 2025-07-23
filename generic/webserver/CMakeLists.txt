cmake_minimum_required (VERSION 3.20.0)

# Set the WASI SDK toolchain file
set(CMAKE_TOOLCHAIN_FILE /opt/wasi-sdk/share/cmake/wasi-sdk-p1.cmake)

set(CMAKE_BUILD_TYPE Release)

set(APPNAME webserver)
project(${APPNAME})

add_compile_options(
#    --target=wasm32-wasm-unknown # Target WebAssembly
    -Os -Wno-unknown-attributes
)

# Set compilation flags
add_link_options(
    -Wl,--strip-all             # Strip all symbol/debug information from the binary
    -Wl,--allow-undefined       # Let the ocre api functions be undefined
    -z stack-size=8192
    -Wl,--initial-memory=65536  # Minimum size of linear memory\
    -Wl,--max-memory=65536      # Maximum size of linear memory
)

add_executable(${APPNAME}.wasm main.c mongoose.c wasi_socket_ext.c)

