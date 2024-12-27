#ifndef NOB_WASM_H
#define NOB_WASM_H

#ifndef CC

#if defined(__GNUC__)
#define CC "gcc"
#elif defined(__clang__)
#define CC "clang"
#elif defined(_MSC_VER)
#define CC "cl"
#else
#define CC "cc"
#endif // defined(__GNUC__)

#endif // CC

#ifndef EMCC
#define EMCC "emcc"
#endif // EMCC

#if defined(_WIN32)

#ifndef EMSDK_ENV
#define EMSDK_ENV "D:/emsdk/emsdk_env.bat"
// #define EMSDK_ENV "C:/Users/antonioroberto/Desktop/w64devkit/emsdk/emsdk_env.bat"
#endif // EMSDK_ENV

#ifndef EMS
#define EMS(cmd) nob_cmd_append((cmd), "set", "EMSDK_QUIET=1", "&&", EMSDK_ENV, "&&");
#endif // EMS

#else
#define EMS(cmd)
#endif // defined(_WIN32)

#ifndef BUILD_DIR
#define BUILD_DIR "build/"
#endif // BUILD_DIR

#ifndef WASM_DIR
#define WASM_DIR "wasm/"
#endif // WASM_DIR

#ifndef BUILD_WASM_DIR
#define BUILD_WASM_DIR BUILD_DIR WASM_DIR
#endif // BUILD_WASM_DIR

#ifndef BIN_DIR
#define BIN_DIR "bin/"
#endif // BIN_DIR

#ifndef SRC_DIR
#define SRC_DIR "src/"
#endif // SRC_DIR

#ifndef LIB_DIR
#define LIB_DIR "lib/"
#endif // LIB_DIR

#ifndef EXTERN_DIR
#define EXTERN_DIR "extern/"
#endif // EXTERN_DIR

#endif // NOB_WASM_H
