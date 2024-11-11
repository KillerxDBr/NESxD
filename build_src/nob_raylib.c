#ifndef NOB_H_
#include "../include/nob.h"
#endif // NOB_H_

#if defined(__GNUC__)
#define CC "gcc"
#elif defined(__clang__)
#define CC "clang"
#elif defined(_MSC_VER)
#define CC "cl"
#else
#define CC "cc"
#endif // defined(__GNUC__)

#define EMCC "emcc"

#if defined(_WIN32)
// #define EMSDK_ENV "D:/emsdk/emsdk_env.bat"
#define EMSDK_ENV "C:/Users/antonioroberto/Desktop/w64devkit/emsdk/emsdk_env.bat"

#define EMS(cmd)                                                                                                                           \
    do {                                                                                                                                   \
        nob_cmd_append((cmd), "set", "EMSDK_QUIET=1");                                                                                     \
        nob_cmd_append((cmd), "&&");                                                                                                       \
        nob_cmd_append((cmd), EMSDK_ENV);                                                                                                  \
        nob_cmd_append((cmd), "&&");                                                                                                       \
    } while (0)
#else
#define EMS(cmd)
#endif // defined(_WIN32)

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
                      "-ggdb3",                                            \
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

#define BUILD_DIR "build/"
#define WASM_DIR "wasm/"
#define BUILD_WASM_DIR BUILD_DIR WASM_DIR
#define BIN_DIR "bin/"
#define SRC_DIR "src/"
#define LIB_DIR "lib/"
#define EXTERN_DIR "extern/"

typedef Nob_File_Paths Objects;

extern Objects obj;
extern const char *skippingMsg;

bool buildRayLib(bool isWeb) {
    const size_t checkpoint = nob_temp_save();
    Nob_Cmd cmd = { 0 };
    Nob_Cmd webCmd = { 0 };
    Nob_Procs procs = { 0 };
    Objects deps = { 0 };
    Nob_String_Builder sb = { 0 };
    Nob_String_Builder cmdRender = { 0 };

    if (isWeb) {
        // if (nob_file_exists(EMSDK_ENV) < 1)
        //     NOB_UNREACHABLE("Could not find EMSDK_ENV: '"EMSDK_ENV"'");

        EMS(&cmd);

        nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never");
        nob_cmd_append(&cmd, RAYLIB_WFLAGS);

        nob_sb_append_cstr(&sb, BUILD_WASM_DIR);
    } else {
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never");
        nob_cmd_append(&cmd, RAYLIB_CFLAGS);

#ifdef USE_SDL
        nob_cmd_append(&cmd, "-I" SDL_PATH);
#endif

        nob_sb_append_cstr(&sb, BUILD_DIR);
    }

    nob_cmd_append(&cmd, RL_INCLUDE_PATHS);
    // nob_cmd_append(&cmd, "-static-libgcc", "-lopengl32", "-lgdi32", "-lwinmm");

    // extern/raylib-5.0/src/config.h
    nob_da_append(&deps, RAYLIB_SRC_PATH "config.h");

    const size_t cmdCount = cmd.count;
    const size_t depsCount = deps.count;
    const size_t sbCount = sb.count;

    // rcore.o =======================================================
    // rcore.c raylib.h rlgl.h utils.h raymath.h rcamera.h rgestures.h
    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "rcore.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "rlgl.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "utils.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raymath.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "rcamera.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "rgestures.h");

    nob_sb_append_cstr(&sb, "rcore.o");
    nob_sb_append_null(&sb);

    // nob_log(NOB_ERROR, "TESTE");

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating rcore.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "rcore.c");

        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));

    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }
    if (!isWeb) {
        // rglfw.o =======================================================
        // rglfw.c

        cmd.count = cmdCount;
        deps.count = depsCount;
        sb.count = sbCount;

        nob_da_append(&deps, RAYLIB_SRC_PATH "rglfw.c");

        nob_sb_append_cstr(&sb, "rglfw.o");
        nob_sb_append_null(&sb);

        nob_da_append(&obj, strdup(sb.items));
        if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
            nob_log(NOB_INFO, "--- Generating rglfw.o ---");
            nob_cmd_append(&cmd, "-o", sb.items);
            nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "rglfw.c");

            if (isWeb) {
                webCmd.count = 0;
                cmdRender.count = 0;
                nob_cmd_render(cmd, &cmdRender);
                nob_sb_append_null(&cmdRender);
                nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
                nob_da_append(&procs, nob_cmd_run_async(webCmd));
            } else
                nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, sb.items);
        }
    }
    // rshapes.o =====================================================
    // rshapes.c raylib.h rlgl.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "rshapes.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "rlgl.h");

    nob_sb_append_cstr(&sb, "rshapes.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating rshapes.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "rshapes.c");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }

    // rtextures.o ===================================================
    // rtextures.c raylib.h rlgl.h utils.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "rtextures.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "rlgl.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "utils.h");

    nob_sb_append_cstr(&sb, "rtextures.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating rtextures.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "rtextures.c");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }

    // rtext.o =======================================================
    // rtext.c raylib.h utils.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "rtext.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "utils.h");

    nob_sb_append_cstr(&sb, "rtext.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating rtext.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "rtext.c");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }

    // utils.o =======================================================
    // utils.c utils.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "utils.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "utils.h");

    nob_sb_append_cstr(&sb, "utils.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating utils.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "utils.c");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }

    // rmodels.o =====================================================
    // rmodels.c raylib.h rlgl.h raymath.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "rmodels.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "rlgl.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raymath.h");

    nob_sb_append_cstr(&sb, "rmodels.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating rmodels.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "rmodels.c");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }

    // raudio.o ======================================================
    // raudio.c raylib.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    nob_da_append(&deps, RAYLIB_SRC_PATH "raudio.c");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");

    nob_sb_append_cstr(&sb, "raudio.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating raudio.o ---");
        nob_cmd_append(&cmd, "-o", sb.items);
        nob_cmd_append(&cmd, "-c", RAYLIB_SRC_PATH "raudio.c");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }

    // raygui.o ======================================================
    // raygui.h

    cmd.count = cmdCount;
    deps.count = depsCount;
    sb.count = sbCount;

    // include/raygui.h
    nob_da_append(&deps, "include/raygui.h");
    nob_da_append(&deps, RAYLIB_SRC_PATH "raylib.h");

    nob_sb_append_cstr(&sb, "raygui.o");
    nob_sb_append_null(&sb);

    nob_da_append(&obj, strdup(sb.items));
    if (nob_needs_rebuild(sb.items, deps.items, deps.count) != 0) {
        nob_log(NOB_INFO, "--- Generating raygui.o ---");
        nob_cmd_append(&cmd, "-xc", "-o", sb.items);
        nob_cmd_append(&cmd, "-c", "include/raygui.h", "-DRAYGUI_IMPLEMENTATION");
        if (isWeb) {
            webCmd.count = 0;
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            nob_da_append(&procs, nob_cmd_run_async(webCmd));
        } else
            nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, sb.items);
    }
    // nob_cmd_append(&cmd, CC, "-fdiagnostics-color=always", "-xc", "-o", output, "-c", NOB_H_DIR, "-DNOB_IMPLEMENTATION");

    bool result = nob_procs_wait(procs);

    nob_sb_free(sb);
    nob_sb_free(cmdRender);

    nob_cmd_free(cmd);
    nob_cmd_free(webCmd);

    nob_da_free(procs);
    nob_da_free(deps);

    nob_temp_rewind(checkpoint);

    return result;
}
