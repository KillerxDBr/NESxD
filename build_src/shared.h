#pragma once

#if defined(_WIN32)
#define strcasecmp _stricmp
#define strdup     _strdup
#endif

#if defined(_WIN32)
#define W32(T) __declspec(dllimport) T __stdcall
#endif // defined(_WIN32)

#if defined(__GNUC__) && !defined(__clang__)
#define nob_cc(cmd)  nob_cmd_append(cmd, "gcc")
#define nob_cxx(cmd) nob_cmd_append(cmd, "g++")
#ifdef RELEASE
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-O2", "-Wall", "-Wextra")
#else
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-O0", "-ggdb", "-Wall", "-Wextra")
#endif // RELEASE
#define CPL                         "gcc"
#define nob_res(cmd, input, output) nob_cmd_append(cmd, "windres", "-i", (input), "-o", (output), DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE")
#elif defined(__clang__)
#define nob_cc(cmd)  nob_cmd_append(cmd, "clang")
#define nob_cxx(cmd) nob_cmd_append(cmd, "clang++")
#ifdef RELEASE
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-O2", "-Wall", "-Wextra")
#else
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "-O0", "-ggdb", "-Wall", "-Wextra")
#endif // RELEASE
#define CPL                         "clang"
#define nob_res(cmd, input, output) nob_cmd_append(cmd, "llvm-rc", "/FO", (output), DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE",  (input))
#elif defined(_MSC_VER)
#define nob_cc(cmd)  nob_cmd_append(cmd, "cl")
#define nob_cxx(cmd) nob_cmd_append(cmd, "cl")
#ifdef RELEASE
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "/W4", "/O2", "/nologo", "/utf-8")
#else
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "/W4", "/Od", "/nologo", "/utf-8")
#endif // RELEASE
#define CPL                         "msvc"
#define nob_res(cmd, input, output) nob_cmd_append(cmd, "rc", "/FO", (output), DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE", "/nologo", (input))
#else
#define nob_cc(cmd)  nob_cmd_append(cmd, "cc")
#define nob_cxx(cmd) nob_cmd_append(cmd, "c++")
#define nob_cc_flags(...)
#define CPL "cc"
#endif // defined(__GNUC__) && !defined(__clang__)

#define NOSHELLDLL
#define NOB_IMPLEMENTATION
#include "../include/nob.h"

#if defined(_MSC_VER) && !defined(__clang__)

#define NOLINK_FLAG "/c"
#define DEF_FLAG    "/D"
#define OBJ_EXT     ".obj"
#define C_MODE      "/TC"
#define CPP_MODE    "/TP"
#define STDCPP      "/std:c++20"

#define nob_cc_obj(cmd, obj)                nob_cmd_append(cmd, nob_temp_sprintf("/Fo:%s", (obj)))
#define nob_cc_include(cmd, path)           nob_cmd_append(cmd, "/I", (path))
#define nob_cc_output_obj(cmd, output_path) nob_cmd_append(cmd, nob_temp_sprintf("/Fo:%s", (output_path)))
#define nob_cc_depfile(cmd, depfile)        nob_cmd_append(cmd, "/showIncludes")
#define nob_cc_include(cmd, path)           nob_cmd_append(cmd, "/I", (path))
#define nob_lib(lib)                        nob_temp_sprintf("%s.lib", lib)

#pragma comment(lib, "shell32.lib") // CommandLineToArgvW in nob_win32_uft8_cmdline_args
#pragma comment(lib, "Shlwapi.lib") // PathIsRelativeW in get_MSVC_default_paths

#else

#define NOLINK_FLAG "-c"
#define DEF_FLAG    "-D"
#define OBJ_EXT     ".o"
#define C_MODE      "-xc"
#define CPP_MODE    "-xc++"
#define STDCPP      "-std=c++20"

#define nob_cc_obj(cmd, obj)                nob_cmd_append(cmd, "-o", (obj))
#define nob_cc_include(cmd, path)           nob_cmd_append(cmd, "-I", (path))
#define nob_cc_output_obj(cmd, output_path) nob_cmd_append(cmd, "-o", (output_path))
#define nob_cc_depfile(cmd, depfile)        nob_cmd_append(cmd, "-MMD", "-MF", (depFile));
#define nob_cc_include(cmd, path)           nob_cmd_append(cmd, "-I", (path))
#define nob_lib(lib)                        nob_temp_sprintf("-l%s", lib)

#endif

#define INC_DIR "include/"
#define SRC_DIR "src/"
#define BLD_DIR "build/"
#define BIN_DIR "bin/"
#define EXT_DIR "extern/"

#if defined(_MSC_VER) && !defined(__clang__)

#ifndef MSVC_DEP_STRING

// #define MSVC_DEP_STRING "Observação: incluindo arquivo:" // pt-BR
#define MSVC_DEP_STRING "Note: including file:" // en-US

#endif // MSVC_DEP_STRING

static const Nob_String_View msvc_prefix = SV_STATIC(MSVC_DEP_STRING);
#endif // defined(_MSC_VER) && !defined(__clang__)

#ifdef _WIN32
#define EXENAME "NESxD.exe"
#else
#define EXENAME "NESxD"
#endif
