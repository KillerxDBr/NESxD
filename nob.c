#include <assert.h>
#include <stdbool.h>
#include <string.h>

bool precompileHeader(void);
bool CompileFiles(void);
bool CleanupFiles(void);
bool CompileDependencies(void);

#define NOB_IMPLEMENTATION
#include "include/nob.h"

#define RAYLIB "./lib/libraylib.a"

#define BUILD_DIR "build"
#define BIN_DIR "bin"
#define SRC_DIR "src"
#define EXTERN_DIR "extern"
#define PCH_SUFFIX "_pch.h"
#define GCH_SUFFIX "_pch.h.gch"

typedef struct {
    const char *c;
    const char *h;
    const char *pch;
} sourceFile;

#define INCLUDES "-Ibuild/", "-I.", "-Iinclude", "-Isrc", "-Istyles", "-Iextern"

#define LIBS "-lgdi32", "-lwinmm", "-lcomdlg32", "-lole32"

const char *files[] = {
    "main", "6502", "config", "gui", "input",
};

#define SORCE_FILES_SIZE sizeof(files) / sizeof(files[0])

const char *skippingMsg = "    File '%s' already up to date, skipping...";

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    const char *program = nob_shift_args(&argc, &argv);

    // nob_log(NOB_INFO, "--- STAGE 1 ---");

    const char *command;
    if (argc <= 0)
        command = "build";
    else
        command = nob_shift_args(&argc, &argv);

    if (strcmp(command, "build") == 0) {
        nob_log(NOB_INFO, "--- Building ---");

        if (!nob_mkdir_if_not_exists(BUILD_DIR))
            return 1;

        nob_log(NOB_INFO, "Building Dependencies...");
        if (!CompileDependencies())
            return 1;

        nob_log(NOB_INFO, "Precompiling Headers...");
        if (!precompileHeader())
            return 1;

        nob_log(NOB_INFO, "Generating Object Files...");
        if (!CompileFiles())
            return 1;

    } else if (strcmp(command, "bundler") == 0) {
        nob_log(NOB_INFO, "--- Bundler ---");
        assert(0 && "Not Implemented!");

    } else if (strcmp(command, "clean") == 0) {
        nob_log(NOB_INFO, "--- Cleaning Files ---");
        if (!CleanupFiles())
            return 1;
        assert(0 && "Not Implemented!");

    } else {
        nob_log(NOB_INFO, "Unknown command, expects: <build, bundler, clean>");
    }

    return 0;
}

// clang-format off

// CFlags
#define CFLAGS                                                                                          \
        "-Wall",                                                                                        \
        "-Wextra",                                                                                      \
        "-Winvalid-pch",                                                                                \
        "-std=gnu11",                                                                                   \
        "-Og",                                                                                          \
        "-g3",                                                                                          \
        "-ggdb",                                                                                        \
        "-march=native",                                                                                \
        "-DKXD_DEBUG",                                                                                  \
        "-D_UNICODE",                                                                                   \
        "-DUNICODE",                                                                                    \
        "-DPLATFORM_DESKTOP",                                                                           \
        "-DGRAPHICS_API_OPENGL_33",                                                                     \
        "-DNES"

// clang-format on

#define STR_SIZE 32ULL

bool precompileHeader(void) {
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    nob_cmd_append(&cmd, "ccache", "gcc", "-fdiagnostics-color=always", "-xc-header", CFLAGS, INCLUDES);
    size_t command_size = cmd.count;

    /* ccache gcc -fdiagnostics-color=always -xc-header -Ibuild/
    -I. -Iinclude -Isrc -Istyles -Iextern -o build/6502precomp.h.gch
    src/6502.h -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb -march=native
    -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
    */
    //-o build/6502precomp.h.gch src/6502.h
    Nob_String_Builder sb = { 0 };
    for (size_t i = 0; i < SORCE_FILES_SIZE; i++) {
        cmd.count = command_size;

        char input[STR_SIZE] = { 0 };
        char output[STR_SIZE] = { 0 };

        // input file
        sb.count = 0;
        nob_sb_append_cstr(&sb, SRC_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, ".h");
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(input, sb.items);

        // output
        sb.count = 0;
        nob_sb_append_cstr(&sb, BUILD_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, GCH_SUFFIX);
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(output, sb.items);

        if (nob_needs_rebuild1(output, &input[0])) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-o", output, input);
            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
    }
    nob_cmd_free(cmd);
    nob_sb_free(sb);
    bool result = nob_procs_wait(procs);
    NOB_FREE(procs.items);
    return result;

defer:
    nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    return false;
}

bool CleanupFiles(void) {
    Nob_Cmd cmd = { 0 };
    // remove-item -WhatIf * -Include *.exe, *.gch, *.log, *.o, *.ini -Recurse -Exclude *.c, *.h, *.py
    nob_cmd_append(&cmd, "pwsh.exe", "-command", "remove-item", "*", "-Include", "nesxd.exe", ",", "*.gch", ",", "*.log", ",", "*.o", ",",
                   "*.ini", "-Recurse", "-Exclude", "nob.exe", ",", "*.c", ",", "*.h", ",", "*.py");
    nob_cmd_run_sync(cmd);
    exit(0);
}

bool CompileFiles(void) {
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    nob_cmd_append(&cmd, "ccache", "gcc", "-fdiagnostics-color=always", "-xc", CFLAGS, INCLUDES);
    size_t command_size = cmd.count;

    /*
    ccache gcc -fdiagnostics-color=always -include build/6502precomp.h
    -Ibuild/ -I. -Iinclude -Isrc -Istyles -Iextern -o build/6502.o
    -c src/6502.c -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb
    -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -DNES
    */

    Nob_String_Builder sb = { 0 };
    for (size_t i = 0; i < SORCE_FILES_SIZE; i++) {
        cmd.count = command_size;

        char input[STR_SIZE] = { 0 };
        char pch[STR_SIZE] = { 0 };
        char gch[STR_SIZE] = { 0 };
        char output[STR_SIZE] = { 0 };

        // input
        sb.count = 0;
        nob_sb_append_cstr(&sb, SRC_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, ".c");
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(input, sb.items);

        // pch
        sb.count = 0;
        nob_sb_append_cstr(&sb, BUILD_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, PCH_SUFFIX);
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(pch, sb.items);

        // gch
        sb.count = sb.count - sizeof(PCH_SUFFIX);

        nob_sb_append_cstr(&sb, GCH_SUFFIX);
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(gch, sb.items);

        // output
        sb.count = 0;
        nob_sb_append_cstr(&sb, BUILD_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, ".o");
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(output, sb.items);

        // nob_log(NOB_INFO, "---------%s---------", files[i]);
        // nob_log(NOB_INFO, "gch: %s", gch);
        // nob_log(NOB_INFO, "pch: %s", pch);
        // nob_log(NOB_INFO, "input: %s", input);
        // nob_log(NOB_INFO, "output: %s", output);

        const char *input_files[] = { input, gch };

        if (nob_needs_rebuild(output, input_files, 2)) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-o", output, "-include", pch, "-c", input);
            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
    }
    // nob_log(NOB_INFO, "------------------");

    // windres -i resource.rc -o build/resource.o
    const char *resource = BUILD_DIR "/resource.o";
    const char *rc_input = "resource.rc";

    if (nob_needs_rebuild(resource, &rc_input, 1)) {
        nob_log(NOB_INFO, "    Building %s file", rc_input);
        cmd.count = 0;
        nob_cmd_append(&cmd, "windres", "-i", rc_input, "-o", resource);
        nob_da_append(&procs, nob_cmd_run_async(cmd));
    } else {
        nob_log(NOB_INFO, skippingMsg, rc_input);
    }

    bool result = nob_procs_wait(procs);
    nob_cmd_free(cmd);
    nob_sb_free(sb);
    NOB_FREE(procs.items);
    return result;
defer:
    nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    return false;
}

bool CompileDependencies(void) {

#if 0
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    bool result = nob_procs_wait(procs);
    nob_cmd_free(cmd);
    NOB_FREE(procs.items);
    return result;
#else
    nob_log(NOB_ERROR, "Compile Dependencies Not implemented, Skipping...");
    return true;
#endif
}
