#ifndef NOB_H_
#include "../include/nob.h"
#endif // NOB_H_

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

#ifdef RELEASE
#define IG_CFLAGS "-O3", "-Wall", "-Wextra"
#else
#define IG_CFLAGS "-Og", "-ggdb", "-Wall", "-Wextra"
#endif // RELEASE

typedef Nob_File_Paths Objects;

extern Objects obj;
extern const char *skippingMsg;

bool CompileRLImgui(bool isWeb) {
    if (isWeb)
        return true;

    bool result = true;
    Nob_Cmd cmd = { 0 };
    Nob_Procs procs = { 0 };

    size_t startObj = obj.count;

    char *output = strdup(BUILD_DIR "cimgui.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/cimgui.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-o", output, "-c", "extern/cimgui/cimgui.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = strdup(BUILD_DIR "imgui.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-o", output, "-c", "extern/cimgui/imgui/imgui.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = strdup(BUILD_DIR "imgui_demo.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_demo.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-o", output, "-c", "extern/cimgui/imgui/imgui_demo.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = strdup(BUILD_DIR "imgui_draw.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_draw.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-o", output, "-c", "extern/cimgui/imgui/imgui_draw.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = strdup(BUILD_DIR "imgui_tables.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_tables.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-o", output, "-c", "extern/cimgui/imgui/imgui_tables.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = strdup(BUILD_DIR "imgui_widgets.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_widgets.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-o", output, "-c", "extern/cimgui/imgui/imgui_widgets.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = strdup(BUILD_DIR "rlImGui.o");
    if (nob_needs_rebuild1(output, "extern/rlImGui/rlImGui.cpp") > 0) {
        nob_cmd_append(&cmd, "g++", "-Iinclude/", "-Iextern/cimgui/imgui/", "-o", output, "-c", "extern/rlImGui/rlImGui.cpp", IG_CFLAGS);
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(1);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    // for (size_t i = startObj; i < obj.count; i++)
    //     printf("obj[%zu]: '%s'\n", i - startObj + 1, obj.items[i]);

defer:
    nob_cmd_free(cmd);
    return result;
}
