#ifndef NOB_H_
#include "../include/nob.h"
#endif // NOB_H_

#include "nob_shared.h"

#ifndef RAYLIB_SRC_PATH
#define RAYLIB_SRC_PATH "extern/raylib/src/"
#endif // RAYLIB_SRC_PATH

#ifndef RL_MIN_SHELL
#define RL_MIN_SHELL RAYLIB_SRC_PATH "minshell.html"
#endif // RL_MIN_SHELL

#ifndef SDL_PATH
#define SDL_PATH "C:/msys64/ucrt64/include/SDL2"
#endif // SDL_PATH

#ifndef PLATFORM
#ifdef USE_SDL
#define PLATFORM "-DPLATFORM_DESKTOP_SDL"
#else
#define PLATFORM "-DPLATFORM_DESKTOP_GLFW"
#endif /* USE_SDL */
#endif // PLATFORM

// TODO: Test SDL Backend, why not?
#ifndef RAYLIB_CFLAGS
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
#endif // RELEASE
#endif // RAYLIB_CFLAGS

#ifndef RAYLIB_WFLAGS
#define RAYLIB_WFLAGS "-Wall",                                             \
                      "-D_GNU_SOURCE",                                     \
                      "-DPLATFORM_WEB",                                    \
                      "-DGRAPHICS_API_OPENGL_ES2",                         \
                      "-Wno-missing-braces",                               \
                      "-Werror=pointer-arith",                             \
                      "-fno-strict-aliasing",                              \
                      "-std=gnu99",                                        \
                      "-Os"
#endif // RAYLIB_WFLAGS
// clang-format on
#ifndef RL_INCLUDE_PATHS
#define RL_INCLUDE_PATHS "-I" RAYLIB_SRC_PATH, "-I" RAYLIB_SRC_PATH "external/glfw/include"
#endif

extern Nob_File_Paths obj;
extern const char *skippingMsg;

const char *rlFiles[] = {
    "rcore", "rglfw", "rshapes", "rtextures", "rtext", "utils", "rmodels", "raudio",
};

bool BuildRayLib(bool isWeb) {
    const size_t checkpoint = nob_temp_save();

    bool result = true;

    char *input;
    char *output;
    char *depFile;

    Nob_Cmd cmd = { 0 };
    Nob_Cmd webCmd = { 0 };
    Nob_Procs procs = { 0 };
    Nob_File_Paths deps = { 0 };

    if (isWeb) {
        EMS(&cmd);

        nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never");
        nob_cmd_append(&cmd, RAYLIB_WFLAGS);
    } else {
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never");
        nob_cmd_append(&cmd, RAYLIB_CFLAGS);

#ifdef USE_SDL
        nob_cmd_append(&cmd, "-I" SDL_PATH);
#endif
    }

    nob_cmd_append(&cmd, RL_INCLUDE_PATHS);

    const size_t cmdCount = cmd.count;

    for (size_t i = 0; i < NOB_ARRAY_LEN(rlFiles); ++i) {
        if (isWeb && strcmp(rlFiles[i], "rglfw") == 0)
            continue;

        cmd.count = cmdCount;
        webCmd.count = 0;
        deps.count = 0;

        input = nob_temp_sprintf("%s%s%s", RAYLIB_SRC_PATH, rlFiles[i], ".c");
        output = nob_temp_sprintf("%s%s%s", isWeb ? BUILD_WASM_DIR : BUILD_DIR, rlFiles[i], ".o");
        depFile = nob_temp_sprintf("%s%s%s", isWeb ? BUILD_WASM_DIR : BUILD_DIR, rlFiles[i], ".o.d");

        if (!ParseDependencyFile(&deps, depFile)) {
            nob_da_append(&deps, input);
            nob_da_append(&deps, RAYLIB_SRC_PATH "config.h");
        }

        if (nob_needs_rebuild(output, deps.items, deps.count) > 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);
            nob_cmd_append(&cmd, "-MMD", "-MF", depFile, "-o", output, "-c", input);

            if (isWeb) {
                Nob_String_Builder cmdRender = { 0 };
                nob_cmd_render(cmd, &cmdRender);
                nob_sb_append_null(&cmdRender);
                nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
                nob_sb_free(cmdRender);
                result = nob_cmd_run_sync_and_reset(&webCmd) && result;
            } else {
                nob_da_append(&procs, nob_cmd_run_async(cmd));
            }

        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }

        output = strdup(output);
        nob_da_append(&obj, output);
    }

    // raygui.h ===========================================
    do {
        cmd.count = cmdCount;
        webCmd.count = 0;
        deps.count = 0;

        input = nob_temp_sprintf("%s%s%s", INC_DIR, "raygui", ".h");
        output = nob_temp_sprintf("%s%s%s", isWeb ? BUILD_WASM_DIR : BUILD_DIR, "raygui", ".o");
        depFile = nob_temp_sprintf("%s%s%s", isWeb ? BUILD_WASM_DIR : BUILD_DIR, "raygui", ".o.d");

        if (!ParseDependencyFile(&deps, depFile)) {
            nob_da_append(&deps, input);
            nob_da_append(&deps, RAYLIB_SRC_PATH "config.h");
        }

        if (nob_needs_rebuild(output, deps.items, deps.count) > 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);
            nob_cmd_append(&cmd, "-MMD", "-MF", depFile, "-xc", "-o", output, "-c", input, "-DRAYGUI_IMPLEMENTATION");

            if (isWeb) {
                Nob_String_Builder cmdRender = { 0 };
                nob_cmd_render(cmd, &cmdRender);
                nob_sb_append_null(&cmdRender);
                nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
                nob_sb_free(cmdRender);
                result = nob_cmd_run_sync_and_reset(&webCmd) && result;
            } else {
                nob_da_append(&procs, nob_cmd_run_async(cmd));
            }

        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }

        output = strdup(output);
        nob_da_append(&obj, output);
    } while (0);

    if (!isWeb)
        result = nob_procs_wait(procs);

    nob_da_free(cmd);
    nob_da_free(webCmd);
    nob_da_free(deps);
    nob_da_free(procs);

    nob_temp_rewind(checkpoint);

    return result;
}
