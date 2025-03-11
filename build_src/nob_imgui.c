#ifndef NOB_H_
#include "../include/nob.h"
#endif // NOB_H_

#include "nob_shared.h"

#ifdef RELEASE
#define IG_CFLAGS "-O3", "-Wall", "-Wextra"
#define IG_WFLAGS "-Os", "-Wall", "-Wextra"
#else
#define IG_CFLAGS "-Og", "-g", "-ggdb", "-Wall", "-Wextra"
#define IG_WFLAGS "-Og", "-g", "-Wall", "-Wextra"
#endif // RELEASE

#define IMGUI_INC "-Iextern/cimgui/", "-Iextern/cimgui/imgui/", "-Iinclude/"

#if 1

const char *rli_files[] = {
    "extern/cimgui/cimgui",           "extern/cimgui/imgui/imgui",        "extern/cimgui/imgui/imgui_demo",
    "extern/cimgui/imgui/imgui_draw", "extern/cimgui/imgui/imgui_tables", "extern/cimgui/imgui/imgui_widgets",
    "extern/rlImGui/rlImGui",
};

bool CompileRLImgui(bool isWeb) {
    bool result = true;

    const size_t cp = nob_temp_save();
    Nob_Cmd cmd = { 0 };
    Nob_Cmd webCmd = { 0 };
    Nob_Procs procs = { 0 };
    Nob_File_Paths deps = { 0 };
    Nob_String_Builder cmdRender = { 0 };

    if (isWeb) {
        EMS(&cmd);
        nob_cmd_append(&cmd, EMXX, "-fdiagnostics-color=never", NO_LINK_FLAG, IG_WFLAGS, IMGUI_INC);

    } else {
        if (hasCCache)
            nob_cmd_append(&cmd, CCACHE);
        nob_cmd_append(&cmd, CXX, "-fdiagnostics-color=never", NO_LINK_FLAG, IG_CFLAGS, IMGUI_INC);
    }

    const size_t cmdSize = cmd.count;
    for (size_t i = 0; i < NOB_ARRAY_LEN(rli_files); ++i) {
        cmd.count = cmdSize;
        deps.count = 0;

        char *input = nob_temp_sprintf("%s%s", rli_files[i], ".cpp");
        char *output = nob_temp_sprintf("%s%s%s", isWeb ? BUILD_WASM_DIR : BUILD_DIR, nob_path_name(rli_files[i]), ".o");
        char *depFile = nob_temp_sprintf("%s%s", output, ".d");

        if (!ParseDependencyFile(&deps, depFile)) {
            nob_da_append(&deps, input);
        }

        if (nob_needs_rebuild(output, deps.items, deps.count) != 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);
            if (isWeb) {
                webCmd.count = 0;
                cmdRender.count = 0;

                nob_cmd_append(&cmd, "-o", output, input);

                nob_cmd_render(cmd, &cmdRender);
                nob_sb_append_null(&cmdRender);

                nob_cmd_append(&webCmd, "cmd.exe", "/c", cmdRender.items);

                result = nob_cmd_run_sync_and_reset(&webCmd) && result;
            } else {
                nob_cmd_append(&cmd, "-MMD", "-MF", depFile, "-o", output, input);
                nob_da_append(&procs, nob_cmd_run_async(cmd));
            }
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
        output = strdup(output);
        nob_da_append(&obj, output);
    }

    if (!isWeb)
        result = nob_procs_wait(procs);

    nob_cmd_free(cmd);
    nob_cmd_free(webCmd);
    nob_da_free(deps);
    nob_sb_free(cmdRender);

    nob_temp_rewind(cp);

    return result;
}
#else
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
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, NO_LINK_FLAG, "extern/cimgui/cimgui.cpp");
        if (isWeb) {
            nob_cmd_append(&cmd, IG_WFLAGS);
            cmdRender.count = 0;
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&webCmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));
            if (!nob_cmd_run_sync_and_reset(&webCmd)) {
                nob_return_defer(false);
            }
            cmd.count = 0;
        } else {
            nob_cmd_append(&cmd, IG_CFLAGS);
            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
        }
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }
    nob_da_append(&obj, output);

    output = isWeb ? strdup(BUILD_WASM_DIR "imgui.o") : strdup(BUILD_DIR "imgui.o");
    if (nob_needs_rebuild1(output, "extern/cimgui/imgui/imgui.cpp") > 0) {
        if (isWeb)
            EMS(&cmd);
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, NO_LINK_FLAG, "extern/cimgui/imgui/imgui.cpp", IG_CFLAGS);
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
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, NO_LINK_FLAG, "extern/cimgui/imgui/imgui_demo.cpp",
                       IG_CFLAGS);
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
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, NO_LINK_FLAG, "extern/cimgui/imgui/imgui_draw.cpp",
                       IG_CFLAGS);
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
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, NO_LINK_FLAG, "extern/cimgui/imgui/imgui_tables.cpp",
                       IG_CFLAGS);
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
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-o", output, NO_LINK_FLAG, "extern/cimgui/imgui/imgui_widgets.cpp",
                       IG_CFLAGS);
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
        nob_cmd_append(&cmd, compiler, "-fdiagnostics-color=never", "-Iinclude/", "-Iextern/cimgui/imgui/", "-o", output, NO_LINK_FLAG,
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
#endif
