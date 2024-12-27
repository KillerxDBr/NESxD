#ifndef NOB_H_
#include "../include/nob.h"
#endif // NOB_H_

#include "nob_wasm.h"

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

#ifdef RELEASE
#define IG_CFLAGS "-O3", "-Wall", "-Wextra"
#else
#define IG_CFLAGS "-Og", "-ggdb", "-Wall", "-Wextra"
#endif // RELEASE

typedef Nob_File_Paths Objects;

extern Objects obj;
extern const char *skippingMsg;

bool CompileRLImgui(bool isWeb) {
    bool result = true;
    const char *compiler = isWeb ? ("em++") : ("g++");
    const size_t temp_cp = nob_temp_save();

    Nob_Cmd cmd = { 0 };
    Nob_Cmd webCmd = { 0 };
    Nob_String_Builder cmdRender = { 0 };

    char *output = isWeb ? strdup(BUILD_WASM_DIR "cimgui.o") : strdup(BUILD_DIR "cimgui.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/cimgui.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, "-c", "extern/cimgui/cimgui.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "imgui.o") : strdup(BUILD_DIR "imgui.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, "-c", "extern/cimgui/imgui/imgui.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "imgui_demo.o") : strdup(BUILD_DIR "imgui_demo.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_demo.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, "-c", "extern/cimgui/imgui/imgui_demo.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "imgui_draw.o") : strdup(BUILD_DIR "imgui_draw.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_draw.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, "-c", "extern/cimgui/imgui/imgui_draw.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "imgui_tables.o") : strdup(BUILD_DIR "imgui_tables.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_tables.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, "-c", "extern/cimgui/imgui/imgui_tables.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "imgui_widgets.o") : strdup(BUILD_DIR "imgui_widgets.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui_widgets.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, "-c", "extern/cimgui/imgui/imgui_widgets.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "rlImGui.o") : strdup(BUILD_DIR "rlImGui.o");
    if (nob_needs_rebuild1(output, "extern/rlImGui/rlImGui.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-Iinclude/", "-Iextern/cimgui/imgui/", "-o", output, "-c",
                       "extern/rlImGui/rlImGui.cpp", IG_CFLAGS);
        if (isWeb) {
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    // for (size_t i = startObj; i < obj.count; i++)
    // printf("obj[%zu]: '%s'\n", i - startObj + 1, obj.items[i]);

defer:
    nob_temp_rewind(temp_cp);

    nob_sb_free(cmdRender);
    nob_cmd_free(cmd);
    nob_cmd_free(webCmd);

    return result;
}
