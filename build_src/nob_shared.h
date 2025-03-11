#pragma once
#ifndef NOB_SHARED_H_
#define NOB_SHARED_H_

extern Nob_File_Paths obj;
const char *skippingMsg = "    File '%s' already up to date, skipping...";

// #define RELEASE
// #define USE_SDL
#define STATIC
#define ROM_PATH "rom/"
#define MEM_BIN_PATH "mem.bin"

#ifndef WS_PORT
#define WS_PORT "8000"
#endif // WS_PORT

#define CCACHE "ccache"

#ifndef CC
#if defined(__GNUC__)
#define CC "gcc"
#define CXX "g++"
#elif defined(__clang__)
#define C "clang"
#define CXX "clang++"
#elif defined(_MSC_VER)
#error Cant Compile with MSVC yet...
#define CC "cl"
#define CXX "cl"

#ifndef MSVC_ENV
#define MSVC_ENV "D:/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat"
#endif // MSVC_ENV

#define MSVC(cmd)                                                                                                                          \
    do {                                                                                                                                   \
        nob_cmd_append((cmd), "cmd", "/c");                                                                                                \
        nob_cmd_append((cmd), MSVC_ENV);                                                                                                   \
        nob_cmd_append((cmd), "&&");                                                                                                       \
    } while (0)
#else
#define CC "cc"
#endif // defined(__GNUC__)
#endif // CC

#if defined(_MSC_VER)
#define NO_LINK_FLAG "/c"
#else
#define NO_LINK_FLAG "-c"
#endif // defined(_MSC_VER)

#ifndef EMCC
#define EMCC "emcc"
#endif // EMCC

#ifndef EMXX
#define EMXX "em++"
#endif // EMXX

#if defined(_WIN32)

#ifndef EMSDK_ENV
#define EMSDK_ENV "D:/emsdk/emsdk_env.bat"
// #define EMSDK_ENV "C:/Users/antonioroberto/Desktop/w64devkit/emsdk/emsdk_env.bat"
#endif // EMSDK_ENV

#define EMS(cmd) nob_cmd_append((cmd), "set", "EMSDK_QUIET=1", "&&", EMSDK_ENV, "&&");

#else
#define EMS(cmd)
#endif // defined(_WIN32)

#define RAYLIB_A "./lib/libraylib.a"

#define BUILD_DIR "build/"
#define WASM_DIR "wasm/"
#define BUILD_WASM_DIR BUILD_DIR WASM_DIR
#define BIN_DIR "bin/"
#define SRC_DIR "src/"
#define LIB_DIR "lib/"
#define EXTERN_DIR "extern/"
#define INC_DIR "include/"

#define INCLUDES "-I.", "-I" BUILD_DIR, "-I" INC_DIR, "-I" SRC_DIR, "-Istyles", "-I" EXTERN_DIR, "-ID:/Projetos/cimgui/imgui"

#define PROGRAM_NAME "nesxd"

#if defined(_WIN32)
#define PY_EXEC "py"
#define EXE_NAME PROGRAM_NAME ".exe"
#else
#define PY_EXEC "python3"
#define EXE_NAME PROGRAM_NAME
#endif // defined(_WIN32)

#define EXE_OUTPUT BIN_DIR EXE_NAME
#define WASM_OUTPUT WASM_DIR PROGRAM_NAME ".html"

#define PCH_SUFFIX "_pch.h"
#define GCH_SUFFIX PCH_SUFFIX ".gch"

#define RAYLIB_SRC_PATH "extern/raylib/src/"

#define RL_MIN_SHELL RAYLIB_SRC_PATH "minshell.html"

#define SDL_PATH "C:/msys64/ucrt64/include/SDL2"

#ifdef USE_SDL
#define PLATFORM "-DPLATFORM_DESKTOP_SDL"
#else
#define PLATFORM "-DPLATFORM_DESKTOP_GLFW"
#endif /* USE_SDL */

// TODO: Test SDL Backend, why not?
// clang-format off
#ifndef RELEASE
#define RAYLIB_CFLAGS "-Wall",                                             \
                      "-D_GNU_SOURCE",                                     \
                      PLATFORM,                                            \
                      "-DGRAPHICS_API_OPENGL_33",                          \
                      "-Wno-missing-braces",                               \
                      "-Werror=pointer-arith",                             \
                      "-fno-strict-aliasing",                              \
                      "-O1",                                               \
                      "-std=c99",                                          \
                      "-Werror=implicit-function-declaration"
#else
#define RAYLIB_CFLAGS "-Wall",                                             \
                      "-D_GNU_SOURCE",                                     \
                      PLATFORM,                                            \
                      "-DGRAPHICS_API_OPENGL_33",                          \
                      "-Wno-missing-braces",                               \
                      "-Werror=pointer-arith",                             \
                      "-fno-strict-aliasing",                              \
                      "-Og",                                               \
                      "-ggdb",                                            \
                      "-std=c99",                                          \
                      "-Werror=implicit-function-declaration"
#endif //RELEASE

#define RAYLIB_WFLAGS "-Wall",                                             \
                      "-D_GNU_SOURCE",                                     \
                      "-DPLATFORM_WEB",                                    \
                      "-DGRAPHICS_API_OPENGL_ES2",                         \
                      "-Wno-missing-braces",                               \
                      "-Werror=pointer-arith",                             \
                      "-fno-strict-aliasing",                              \
                      "-std=gnu99",                                        \
                      "-Os"

// clang-format on
#define RL_INCLUDE_PATHS "-I" RAYLIB_SRC_PATH, "-I" RAYLIB_SRC_PATH "external/glfw/include"

#ifdef _WIN32
#define FINDER "where"
#else
#define FINDER "which"
#endif

#define BOOLLOG(v) nob_log(NOB_INFO, "%s: %s", #v, v ? "True" : "False")

bool hasCCache;

#endif // NOB_SHARED_H_
