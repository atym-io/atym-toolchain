#!/bin/sh

# Create build directory if it doesn't exist
[ -d build ] || mkdir build

/opt/wasi-sdk/bin/clang -I . -o build/echo.wasm ./wasi_socket_ext.c ./server.c -Wl,--strip-all

#wamrc --target=thumbv8m.main --target-abi=eabihf  --cpu=cortex-m33 --xip  -o echo.aot echo.wasm

