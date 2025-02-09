#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _UNICODE
#undef _UNICODE
#endif

#define NOB_IMPLEMENTATION
#include "include/nob.h"

#include <locale.h>
#include <signal.h>

#if defined(_WIN32)
#include <consoleapi2.h>
#endif // defined(_WIN32)

#include "build_src/nob_shared.h"

#if defined(_WIN32)
const char *libs[] = {
    "gdi32", "winmm", "comdlg32", "ole32", "shell32", "Dbghelp",
};
#else
#define LIBS "-lm"
#endif // defined(_WIN32)

const char *files[] = {
    "main", "6502", "config", "gui", "input", "kxdMem", "iconTray", "lang",
#ifndef RELEASE
    "test",
#endif
};

const char *filesFlags[] = {
    "-DKXD_MAIN_FILE",  "-DKXD_6502_FILE", "-DKXD_CONFIG_FILE",   "-DKXD_GUI_FILE",
    "-DKXD_INPUT_FILE", "-DKXD_MEM_FILE",  "-DKXD_ICONTRAY_FILE", "-DKXD_LANG_FILE",
#ifndef RELEASE
    "-DKXD_TEST_FILE",
#endif
};

const char *dependencies[] = {
    "tinyfiledialogs",
#ifdef _WIN32
    "WindowsHeader",
#endif /* _WIN32 */
};

#if !defined(STATIC)
const char *libraries[] = {
    //  "cimgui",
};
#endif

typedef struct {
    const char *file_path;
    size_t offset;
    size_t size;
} Resource;

typedef struct Resources {
    Resource *items;
    size_t count;
    size_t capacity;
} Resources;

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} EmbedFiles;

Nob_File_Paths obj = { 0 };
Resources resources = { 0 };

bool BuildRayLib(bool isWeb);
bool PrecompileHeader(bool isWeb);
bool CompileFiles(bool isWeb);
bool CleanupFiles(void);
bool CompileDependencies(bool isWeb);
bool LinkExecutable(bool isWeb);
bool CompileNobHeader(bool isWeb);
bool Bundler(char **path, size_t pathCount);
bool TestFile(void);
bool WebServer(const char *pyExec);
bool TestCompiler(void);
bool C3Compile(bool isWeb);
bool ExeIsInPath(const char *exe);

typedef struct WinVer {
    unsigned long major;
    unsigned long minor;
    unsigned long build;
} WinVer;

typedef enum {
    OLDER_WIN = -1,
    WIN_XP,
    WIN_VISTA,
    WIN_7,
    WIN_8,
    WIN_81,
    WIN_10,
    WIN_11,
} WVResp;

WVResp GetWindowsVersion(void);
WinVer GetWinVer(void);

#ifdef STATIC
bool staticCompile = true;
#else
bool staticCompile = false;
#endif

#include "build_src/nob_imgui.c"
#include "build_src/nob_raylib.c"

void PrintUsage(void) {
    nob_log(NOB_ERROR, "Incorrect use of program");
    nob_log(NOB_ERROR, "Usage: ./nob <[b]uild [[w]eb] [[c]lean/cls], [w]eb [python executable], bundler [dir], [c]lean/cls>");
}

int main(int argc, char **argv) {
    // Needs to be commented out to DEBUG
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, nob_header_path(), "build_src/nob_shared.h", "build_src/nob_imgui.c", "build_src/nob_raylib.c");

#if defined(_WIN32)
    if (!SetConsoleOutputCP(CP_UTF8)) {
        nob_log(NOB_ERROR, "Could not set console output to 'UTF-8'");
    }

    const WVResp WinVer = GetWindowsVersion();
    if (WinVer >= WIN_10) {
        nob_log(NOB_INFO, "Enabling buffer on console std outputs (Win 10+)");

        // on Windows 10+ we need buffering or console will get 1 byte at a time (screwing up utf-8 encoding)
        if (setvbuf(stderr, NULL, _IOFBF, 1024) != 0) {
            nob_log(NOB_ERROR, "Could not set \"%s\" buffer to %d: %s", "stderr", 1024, strerror(errno));
        }

        if (setvbuf(stdout, NULL, _IOFBF, 1024) != 0) {
            nob_log(NOB_ERROR, "Could not set \"%s\" buffer to %d: %s", "stdout", 1024, strerror(errno));
        }
    }
#endif // defined(_WIN32)

    nob_log(NOB_INFO, "Locale set to \"%s\"",
#ifndef _WIN32 /* Non Windows */
            setlocale(LC_ALL, "C.UTF-8")
#else

#ifdef _UCRT /* ucrtbase.dll Windows */
            setlocale(LC_ALL, ".UTF-8")
#else        /* msvcrt.dll Windows */
            setlocale(LC_ALL, "C")
#endif       /* _UCRT */

#endif /* _WIN32 */
    );
    // nob_log(NOB_INFO, "Locale set to \"%s\"", setlocale(LC_ALL, NULL));

    assert(NOB_ARRAY_LEN(files) == NOB_ARRAY_LEN(filesFlags));

    nob_shift(argv, argc);
    // const char *program = nob_shift(argv, argc);
    // NOB_UNUSED(program);

    nob_log(NOB_INFO, "Using Compiler: \"" CC "\"");
    nob_log(NOB_INFO, "Using WASM Compiler: \"" EMCC "\"");

    hasCCache = ExeIsInPath(CCACHE);

    const char *command;
    if (argc)
        command = nob_shift(argv, argc);
    else
        command = "build";

    int result = 0;

    if (strcmp(command, "build") == 0 || strcmp(command, "b") == 0) {
        nob_log(NOB_INFO, "--- Building ---");

        bool isWeb = false;

        nob_log(NOB_INFO, "Testing Compiler: '" CC "', 'c3c'");
        if (!TestCompiler()) {
            nob_return_defer(1);
        }

        if (argc > 0) {
            command = nob_shift(argv, argc);
            // nob.exe build clean/cls
            if ((strcmp(command, "clean") == 0) || (strcmp(command, "cls") == 0) || (strcmp(command, "c") == 0)) {
                nob_log(NOB_INFO, "    Forcing Rebuild of all files...");
                CleanupFiles();
            } else if (strcmp(command, "web") == 0 || strcmp(command, "w") == 0) {
                if (!nob_mkdir_if_not_exists(BUILD_WASM_DIR))
                    nob_return_defer(1);

                isWeb = true;
                if (argc > 0) {
                    command = nob_shift(argv, argc);
                    if ((strcmp(command, "clean") == 0) || (strcmp(command, "cls") == 0) || (strcmp(command, "c") == 0)) {
                        nob_log(NOB_INFO, "    Forcing Rebuild of all files...");
                        if (!CleanupFiles())
                            nob_return_defer(1);
                    }
                }
            }
        }
#if defined(_WIN32)
        if (isWeb) {
            if (nob_file_exists(EMSDK_ENV) != 1) {
                nob_log(NOB_ERROR, "Could not find emsdk env builder \"" EMSDK_ENV "\", check if the path is correct and try again...");
                nob_return_defer(1);
            }
        }
#endif

        if (!nob_mkdir_if_not_exists(BUILD_DIR))
            nob_return_defer(1);

        const size_t bkpCount = obj.count;
        nob_log(NOB_INFO, "--- Building Raylib ---");
        if (!BuildRayLib(isWeb)) {
            if (nob_file_exists(RAYLIB_A) != 1)
                nob_return_defer(1);

            obj.count = bkpCount;
            nob_da_append(&obj, RAYLIB_A);
        }

        nob_log(NOB_INFO, "--- Building Dependencies ---");
        if (!CompileDependencies(isWeb))
            nob_return_defer(1);

        if (!CompileRLImgui(isWeb))
            nob_return_defer(1);

        if (!isWeb) {
            nob_log(NOB_INFO, "--- Precompiling Headers ---");
            if (!PrecompileHeader(false))
                nob_return_defer(1);
        }

        nob_log(NOB_INFO, "--- Generating Object Files ---");
        if (!CompileFiles(isWeb))
            nob_return_defer(1);

        nob_log(NOB_INFO, "--- Compiling C3 Module ---");
        if (!C3Compile(isWeb)) {
            nob_return_defer(1);
        }

        nob_log(NOB_INFO, "--- Compiling Executable ---");

        if (!nob_mkdir_if_not_exists(BIN_DIR))
            nob_return_defer(1);

        if (isWeb) {
            if (!nob_mkdir_if_not_exists(WASM_DIR))
                nob_return_defer(1);
        }

        if (!LinkExecutable(isWeb))
            nob_return_defer(1);

        nob_log(NOB_INFO, "--- Finished Compiling ---");

        nob_return_defer(0);

        // } else if (strcmp(command, "web") == 0 || strcmp(command, "w") == 0) {
        //     if (!nob_mkdir_if_not_exists(BUILD_DIR))
        //         return 1;

        //     if (!nob_mkdir_if_not_exists(BUILD_WASM_DIR))
        //         return 1;

        //     if (!nob_mkdir_if_not_exists(WASM_DIR))
        //         return 1;

        //     return !BuildWasm();

    } else if (strcmp(command, "bundler") == 0) {
        nob_log(NOB_INFO, "--- Bundler ---");
        nob_return_defer(!Bundler(argv, argc));

    } else if ((strcmp(command, "test") == 0) || (strcmp(command, "t") == 0)) {
        nob_log(NOB_INFO, "--- Building Test File ---");
        nob_return_defer(!TestFile());

    } else if ((strcmp(command, "web") == 0) || (strcmp(command, "w") == 0)) {
        nob_log(NOB_INFO, "--- Starting WebServer ---");

        if (argc > 0)
            command = nob_shift(argv, argc);
        else
            command = PY_EXEC;

        nob_return_defer(!WebServer(command));
    } else if ((strcmp(command, "clean") == 0) || (strcmp(command, "cls") == 0) || (strcmp(command, "c") == 0)) {
        nob_log(NOB_INFO, "--- Cleaning Files ---");
        nob_return_defer(!CleanupFiles());

    } else {
        PrintUsage();
        // nob_log(NOB_INFO, "Unknown command \"%s\", expects: <[b]uild [[w]eb] [[c]lean/cls],  bundler [dir], [c]lean/cls>", command);
        nob_return_defer(1);
    }

    NOB_UNREACHABLE("Main");

defer:
    for (size_t i = 0; i < obj.count; ++i)
        free((void *)obj.items[i]);

    free(obj.items);
    free(resources.items);

    return result;
}

// clang-format off
#ifndef RELEASE
// CFlags
#define CFLAGS                                                                                          \
        "-Wall",                                                                                        \
        "-Wextra",                                                                                      \
		"-Wswitch-enum",                                                                                \
        "-Og",                                                                                          \
        "-ggdb",                                                                                        \
        "-march=native",                                                                                \
        "-DKXD_DEBUG",                                                                                  \
        "-DPLATFORM_DESKTOP",                                                                           \
        "-DGRAPHICS_API_OPENGL_33",                                                                     \
        "-DNES"
#else

// CFlags
#define CFLAGS                                                                                          \
        "-O3",                                                                                          \
        "-march=native",                                                                                \
        "-DPLATFORM_DESKTOP",                                                                           \
        "-DGRAPHICS_API_OPENGL_33",                                                                     \
        "-DNES"
#endif
// clang-format on

// clang-format off
// -Os -Wall -s USE_GLFW=3 -s FORCE_FILESYSTEM=1 -s ASYNCIFY -DPLATFORM_WEB --preload-file resources
#define WFLAGS  "-Og",                                                                   \
                "-g",                                                                    \
                "-Wall",                                                                 \
                "-Wextra",                                                               \
                "-lm",                                                                   \
                "-DPLATFORM_WEB",                                                        \
                "-DKXD_DEBUG",                                                           \
                "-DNES",                                                                 \
                "-sUSE_GLFW=3",                                                          \
                "-sFORCE_FILESYSTEM=1",                                                  \
                "-sALLOW_MEMORY_GROWTH=1",                                               \
                "-sGL_ENABLE_GET_PROC_ADDRESS=1",                                        \
                "-sSTACK_SIZE=100mb",                                                    \
                "-sASSERTIONS=1"

// clang-format on

// #define STR_SIZE 128

bool PrecompileHeader(bool isWeb) {
    if (isWeb)
        return true;
    Nob_File_Paths extraHeaders = { 0 };
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    if (isWeb) {
        EMS(&cmd);
        nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never", "-xc-header", WFLAGS);
    } else {
        if (hasCCache)
            nob_cmd_append(&cmd, CCACHE);
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-xc-header", CFLAGS);
    }

    nob_cmd_append(&cmd, INCLUDES);
    const size_t command_size = cmd.count;

    for (size_t i = 0; i < NOB_ARRAY_LEN(files); i++) {
        size_t internalCheckpoint = nob_temp_save();

        cmd.count = command_size;
        extraHeaders.count = 0;

        char *input = nob_temp_sprintf("%s%s%s", SRC_DIR, files[i], ".h");
        char *output = nob_temp_sprintf("%s%s%s", isWeb ? BUILD_WASM_DIR : BUILD_DIR, files[i], GCH_SUFFIX);

        char *depFile = nob_temp_sprintf("%s.d", output);
        if (!ParseDependencyFile(&extraHeaders, depFile)) {
            nob_da_append(&extraHeaders, input);
        }

        // nob_log(NOB_INFO, "------------------------------------");
        // nob_log(NOB_INFO, "Extra Headers for %s:", input);
        // for (size_t j = 0; j < extraHeaders.count; j++) {
        //     nob_log(NOB_INFO, "    %s", extraHeaders.items[j]);
        // }
        // nob_log(NOB_INFO, "Number of extra headers: %zu", extraHeaders.count);
        // nob_log(NOB_INFO, "------------------------------------");

        if (nob_needs_rebuild(output, extraHeaders.items, extraHeaders.count) != 0) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-MMD", "-MF", depFile, "-o", output, input, filesFlags[i]);

            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
        nob_temp_rewind(internalCheckpoint);
    }
    nob_cmd_free(cmd);

    bool result = nob_procs_wait(procs);

    nob_da_free(extraHeaders);
    nob_da_free(procs);

    return result;

    // defer:
    //     nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    //     return false;
}

bool LinkExecutable(bool isWeb) {
    const size_t checkpoint = nob_temp_save();
    bool result = true;
    Nob_Cmd cmd = { 0 };

    bool missingFile = false;
    for (size_t i = 0; i < obj.count; ++i) {
        if (nob_file_exists(obj.items[i]) != 1) {
            nob_log(NOB_ERROR, "Could not find file '%s'", obj.items[i]);
            missingFile = true;
        }
    }

    if (missingFile) {
        nob_log(NOB_ERROR, "Missing Files Found, aborting...");
        nob_return_defer(false);
    }

    // gcc -o bin/nesxd.exe build/6502.o build/config.o build/gui.o build/input.o build/main.o build/nob.o build/resource.o
    // build/tinyfiledialogs.o lib/libraylib.a -lgdi32 -lwinmm -lcomdlg32 -lole32
    if (isWeb) {
        // Building nesxd wasm, html, js, data
        if (nob_needs_rebuild(WASM_OUTPUT, obj.items, obj.count) != 0) {
            EMS(&cmd);

            nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never");

            nob_cmd_append(&cmd, "-o", WASM_OUTPUT);
            nob_cmd_append(&cmd, WFLAGS);

            if (nob_file_exists(ROM_PATH) == 1) {
                nob_cmd_append(&cmd, "--preload-file", ROM_PATH);
            }

            if (nob_file_exists(MEM_BIN_PATH) == 1) {
                nob_cmd_append(&cmd, "--preload-file", MEM_BIN_PATH);
                // nob_copy_file(MEM_BIN_PATH, WASM_DIR MEM_BIN_PATH); // Copying dont work, file needs to be bundled
            }

            nob_cmd_append(&cmd, "--shell-file", RL_MIN_SHELL);

            nob_da_append_many(&cmd, obj.items, obj.count);

            Nob_String_Builder cmdRender = { 0 };
            nob_cmd_render(cmd, &cmdRender);
            nob_sb_append_null(&cmdRender);

            cmd.count = 0;
            nob_cmd_append(&cmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));

            nob_sb_free(cmdRender);
            result = nob_cmd_run_sync(cmd);
        }
    } else if (nob_needs_rebuild(EXE_OUTPUT, obj.items, obj.count) != 0) {
        if (hasCCache)
            nob_cmd_append(&cmd, CCACHE);
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never");
        nob_cmd_append(&cmd, "-o", EXE_OUTPUT);
        nob_da_append_many(&cmd, obj.items, obj.count);

#ifndef STATIC
        Nob_String_Builder sb = { 0 };
        nob_cmd_append(&cmd, "-L" LIB_DIR);
        nob_sb_append_cstr(&sb, "-l");
        size_t SBcnt = sb.count;
        for (size_t i = 0; i < NOB_ARRAY_LEN(libraries); i++) {
            sb.count = SBcnt;
            nob_sb_append_cstr(&sb, libraries[i]);
            nob_sb_append_null(&sb);
            nob_log(NOB_INFO, "Dynamic Lib: %s", sb.items);
            char *s = strdup(sb.items);
            nob_cmd_append(&cmd, s);
        }
        nob_log(NOB_INFO, "--- Copying .DLL files to executable folder ---");
        sb.count = 0;
        nob_sb_append_cstr(&sb, LIB_DIR);
        SBcnt = sb.count;
        Nob_String_Builder dest = { 0 };
        nob_sb_append_cstr(&dest, BIN_DIR);
        const size_t destCnt = dest.count;
        for (size_t i = 0; i < NOB_ARRAY_LEN(libraries); i++) {
            sb.count = SBcnt;
            nob_sb_append_cstr(&sb, libraries[i]);
            nob_sb_append_cstr(&sb, ".dll");
            nob_sb_append_null(&sb);

            dest.count = destCnt;
            nob_sb_append_cstr(&dest, libraries[i]);
            nob_sb_append_cstr(&dest, ".dll");
            nob_sb_append_null(&dest);

            nob_copy_file(sb.items, dest.items)
        }

        nob_sb_free(dest);
        nob_sb_free(sb);
#endif /* STATIC */
#if defined(_WIN32)
        nob_cmd_append(&cmd, "-L" LIB_DIR);
        Nob_String_Builder sb_libs = { 0 };
        for (size_t i = 0; i < NOB_ARRAY_LEN(libs); i++)
#if defined(_MSC_VER)
        {
            sb_libs.count = 0;
            nob_sb_append_cstr(&sb_libs, libs[i]);
            nob_sb_append_cstr(&sb_libs, ".lib");
            nob_sb_append_null(&sb_libs);
            char *s = strdup(sb_libs.items);
            nob_cmd_append(&cmd, s);
        }
#else  // defined(_MSC_VER)
        {
            sb_libs.count = 0;
            nob_sb_append_cstr(&sb_libs, "-l");
            nob_sb_append_cstr(&sb_libs, libs[i]);
            nob_sb_append_null(&sb_libs);
            char *s = strdup(sb_libs.items);
            nob_cmd_append(&cmd, s);
        }
#endif // defined(_MSC_VER)
        nob_sb_free(sb_libs);
#else  // defined(_WIN32)
        nob_cmd_append(&cmd, CFLAGS, LIBS);
#endif // defined(_WIN32)

#ifdef USE_SDL
#if defined(_MSC_VER)
        nob_cmd_append(&cmd, "SDL2.lib");
#else
        nob_cmd_append(&cmd, "-lSDL2");
#endif // defined(_MSC_VER)
#endif /* USE_SDL */

        nob_cmd_append(&cmd, "-static", "-lstdc++");
        result = nob_cmd_run_sync(cmd);

    } else {
        nob_log(NOB_INFO, skippingMsg, EXE_OUTPUT);
    }

defer:
    nob_cmd_free(cmd);

    nob_temp_rewind(checkpoint);
    return result;
}

bool CompileFiles(bool isWeb) {
    const size_t checkpoint = nob_temp_save();

    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };
    Nob_Cmd webCmd = { 0 };
    Nob_String_Builder cmdRender = { 0 };
    Nob_String_Builder sb = { 0 };

    if (isWeb) {
        EMS(&cmd);
        nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never", "-xc", WFLAGS);
    } else {
        if (hasCCache)
            nob_cmd_append(&cmd, CCACHE);
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-xc", CFLAGS);
    }

    nob_cmd_append(&cmd, INCLUDES);
    const size_t command_size = cmd.count;

    /*
    ccache gcc -fdiagnostics-color=always -include build/6502precomp.h
    -Ibuild/ -I. -Iinclude -Isrc -Istyles -Iextern -o build/6502.o
    -c src/6502.c -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb
    -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -DNES
    */

    for (size_t i = 0; i < NOB_ARRAY_LEN(files); i++) {
        size_t internalCheckpoint = nob_temp_save();
        cmd.count = command_size;

        char *input;
        char *pch;
        char *gch;
        char *output;

        // input
        sb.count = 0;
        nob_sb_append_cstr(&sb, SRC_DIR);
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, ".c");
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(input, sb.items);

        input = nob_temp_strdup(sb.items);

        // if (!isWeb) {
        // pch
        sb.count = 0;
        if (isWeb)
            nob_sb_append_cstr(&sb, BUILD_WASM_DIR);
        else
            nob_sb_append_cstr(&sb, BUILD_DIR);

        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, PCH_SUFFIX);
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(pch, sb.items);

        pch = nob_temp_strdup(sb.items);

        // gch
        sb.count = sb.count - sizeof(PCH_SUFFIX);

        nob_sb_append_cstr(&sb, GCH_SUFFIX);
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(gch, sb.items);

        gch = nob_temp_strdup(sb.items);

        // }
        // output
        sb.count = 0;
        if (isWeb)
            nob_sb_append_cstr(&sb, BUILD_WASM_DIR);
        else
            nob_sb_append_cstr(&sb, BUILD_DIR);

        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, ".o");
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(output, sb.items);

        output = nob_temp_strdup(sb.items);

        // nob_log(NOB_INFO, "---------%s---------", files[i]);
        // nob_log(NOB_INFO, "gch: %s", gch);
        // nob_log(NOB_INFO, "pch: %s", pch);
        // nob_log(NOB_INFO, "input: %s", input);
        // nob_log(NOB_INFO, "output: %s", output);

        const char *input_files[] = { input, gch };

        int input_count = isWeb ? 1 : 2;

        if (nob_needs_rebuild(output, input_files, input_count) != 0) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-o", output);

            if (!isWeb)
                nob_cmd_append(&cmd, "-include", pch);

            nob_cmd_append(&cmd, NO_LINK_FLAG, input, filesFlags[i]);

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
            nob_log(NOB_INFO, skippingMsg, output);
        }
        // char *toObj = malloc(strlen(output) + 1);
        // strcpy(toObj, output);
        // nob_da_append(&obj, toObj);
        output = strdup(output);
        nob_da_append(&obj, output);

        nob_temp_rewind(internalCheckpoint);
    }
    // nob_log(NOB_INFO, "------------------");

    // windres -i resource.rc -o build/resource.o
#if defined(_WIN32)
    if (!isWeb) {
        const char *rc_input = "resource.rc";
        const char *manifest = "manifest.xml";
        if (nob_file_exists(rc_input) == 1 && nob_file_exists(manifest) == 1) {
            const char *resource = BUILD_DIR "resource.o";
            const char *inputs[] = { rc_input, manifest };

            if (nob_needs_rebuild(resource, inputs, 2) != 0) {
                nob_log(NOB_INFO, "--- Building %s file", rc_input);
                cmd.count = 0;
                if (hasCCache)
                    nob_cmd_append(&cmd, CCACHE);
                nob_cmd_append(&cmd, "windres", "-i", rc_input, "-o", resource);
                nob_da_append(&procs, nob_cmd_run_async(cmd));
            } else {
                nob_log(NOB_INFO, skippingMsg, resource);
            }
            resource = strdup(resource);
            nob_da_append(&obj, resource);
        }
    }
#endif
    bool result = nob_procs_wait(procs);

    nob_cmd_free(cmd);
    nob_cmd_free(webCmd);

    nob_sb_free(sb);
    nob_sb_free(cmdRender);

    NOB_FREE(procs.items);

    nob_temp_rewind(checkpoint);

    return result;

    // defer:
    //     nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    //     return false;
}

bool CompileDependencies(bool isWeb) {
    Nob_String_Builder sb = { 0 };
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    /*
    gcc -fdiagnostics-color=always -Ibuild/ -I. -Iinclude -Isrc -Istyles -Iextern -o bin/nesxd.exe build/main.o build/6502.o build/input.o
    build/config.o build/gui.o build/nob.o build/tinyfiledialogs.o build/resource.o -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb
    -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 ./lib/libraylib.a -lgdi32 -lwinmm -lcomdlg32
    -lole32 -DKXD_DEBUG*/
    if (isWeb) {
        EMS(&cmd);
        nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never", "-xc", WFLAGS);
    } else {
        if (hasCCache)
            nob_cmd_append(&cmd, CCACHE);
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-xc", CFLAGS);
    }

    nob_cmd_append(&cmd, INCLUDES);

    const size_t command_size = cmd.count;
    for (size_t i = 0; i < NOB_ARRAY_LEN(dependencies); ++i) {
        if (isWeb) {
            if (strcmp(dependencies[i], "tinyfiledialogs") == 0 || strcmp(dependencies[i], "WindowsHeader") == 0)
                continue;
        }

        const size_t checkpoint = nob_temp_save();

        cmd.count = command_size;
        char *input;
        char *output;

        // input
        sb.count = 0;
        nob_sb_append_cstr(&sb, EXTERN_DIR);
        nob_sb_append_cstr(&sb, dependencies[i]);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, dependencies[i]);
        nob_sb_append_cstr(&sb, ".c");
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(input, sb.items);

        input = nob_temp_strdup(sb.items);

        // pch
        sb.count = 0;
        nob_sb_append_cstr(&sb, BUILD_DIR);
        nob_sb_append_cstr(&sb, dependencies[i]);
        nob_sb_append_cstr(&sb, ".o");
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(output, sb.items);

        output = nob_temp_strdup(sb.items);

        // nob_log(NOB_INFO, "Input: %s", input);
        // nob_log(NOB_INFO, "Output: %s", output);

        if (nob_needs_rebuild1(output, input) != 0) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-o", output, NO_LINK_FLAG, input);
            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
        output = strdup(output);
        nob_da_append(&obj, output);

        nob_temp_rewind(checkpoint);
    }

    bool result = nob_procs_wait(procs);

    nob_cmd_free(cmd);
    nob_da_free(procs);
    nob_sb_free(sb);

    return CompileNobHeader(isWeb) & result;
    // defer:
    //     nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    //     return false;
}

bool CompileNobHeader(bool isWeb) {
    bool result = true;
    const size_t checkpoint = nob_temp_save();

    Nob_Cmd cmd = { 0 };

    const char *nob_header_dir = nob_header_path();
    const char *output;

    if (isWeb)
        output = BUILD_WASM_DIR "nob.o";
    else
        output = BUILD_DIR "nob.o";

    if (isWeb) {
        if (nob_needs_rebuild1(output, nob_header_dir) != 0) {
            nob_log(NOB_INFO, "Rebuilding: '%s'", output);
            EMS(&cmd);
            nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never", "-xc", "-Os", "-Wall", "-Wextra", "-sFORCE_FILESYSTEM=1",
                           "-sALLOW_MEMORY_GROWTH=1", "-sGL_ENABLE_GET_PROC_ADDRESS=1", "-sASSERTIONS=1");
            nob_cmd_append(&cmd, "-o", output, NO_LINK_FLAG, nob_header_dir, "-DNOB_IMPLEMENTATION");

            Nob_String_Builder cmdRender = { 0 };

            nob_cmd_render(cmd, &cmdRender);
            cmd.count = 0;
            nob_sb_append_null(&cmdRender);
            nob_cmd_append(&cmd, "cmd.exe", "/c", nob_temp_strdup(cmdRender.items));

            // cmdRender.count = 0;
            // nob_cmd_render(cmd, &cmdRender);
            // nob_sb_append_null(&cmdRender);
            // nob_log(NOB_INFO, "Command: %s", cmdRender.items);

            nob_sb_free(cmdRender);

            if (!nob_cmd_run_sync(cmd))
                nob_return_defer(false);

        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
    } else {
        if (nob_needs_rebuild1(output, nob_header_dir) != 0) {
            nob_log(NOB_INFO, "Rebuilding: '%s'", output);
            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);
            nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-xc", "-o", output, NO_LINK_FLAG, nob_header_dir,
                           "-DNOB_IMPLEMENTATION");

            if (!nob_cmd_run_sync(cmd))
                nob_return_defer(false);
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
    }
    output = strdup(output);
    nob_da_append(&obj, output);

defer:
    nob_temp_rewind(checkpoint);
    nob_cmd_free(cmd);
    return result;
}

#define genf(out, ...)                                                                                                                     \
    do {                                                                                                                                   \
        fprintf((out), __VA_ARGS__);                                                                                                       \
        fprintf((out), " // %s:%d\n", __FILE__, __LINE__);                                                                                 \
    } while (0)

bool generate_resource_bundle(void) {
    bool result = true;
    Nob_String_Builder bundle = { 0 };
    Nob_String_Builder content = { 0 };
    FILE *out = NULL;

    // bundle  = [aaaaaaaaabbbbb]
    //            ^        ^
    // content = []
    // 0, 9

    for (size_t i = 0; i < resources.count; ++i) {
        content.count = 0;
        if (!nob_read_entire_file(resources.items[i].file_path, &content))
            nob_return_defer(false);
        resources.items[i].offset = bundle.count;
        resources.items[i].size = content.count;
        nob_da_append_many(&bundle, content.items, content.count);
        nob_da_append(&bundle, 0);
    }

    const char *bundle_h_path = "./src/bundle.h";
    out = fopen(bundle_h_path, "wb");
    if (out == NULL) {
        nob_log(NOB_ERROR, "Could not open file %s for writing: %s", bundle_h_path, strerror(errno));
        nob_return_defer(false);
    }

    genf(out, "#pragma once");
    genf(out, "#ifndef BUNDLE_H_");
    genf(out, "#define BUNDLE_H_");
    genf(out, "#include <stdlib.h>");
    genf(out, "typedef struct {");
    genf(out, "    const char *file_path;");
    genf(out, "    size_t offset;");
    genf(out, "    size_t size;");
    genf(out, "} Resource;");
    genf(out, "size_t resources_count = %zu;", resources.count);
    genf(out, "Resource resources[] = {");
    for (size_t i = 0; i < resources.count; ++i) {
        genf(out, "    {.file_path = \"%s\", .offset = %zu, .size = %zu},", resources.items[i].file_path, resources.items[i].offset,
             resources.items[i].size);
    }
    genf(out, "};");

    genf(out, "unsigned char bundle[] = {");
    size_t row_size = 20;
    for (size_t i = 0; i < bundle.count;) {
        fprintf(out, "     ");
        for (size_t col = 0; col < row_size && i < bundle.count; ++col, ++i) {
            fprintf(out, "0x%02X, ", (unsigned char)bundle.items[i]);
        }
        genf(out, " ");
    }
    genf(out, "};");
    genf(out, "#endif // BUNDLE_H_");

    nob_log(NOB_INFO, "Generated %s", bundle_h_path);

defer:
    if (out)
        fclose(out);
    nob_sb_free(content);
    nob_sb_free(bundle);
    return result;
}

void recurse_dir(Nob_String_Builder *sb, EmbedFiles *eb) {
    Nob_File_Paths children = { 0 };
    for (size_t i = 0; i < sb->count; i++) {
        if (sb->items[i] == '\\')
            sb->items[i] = '/';
    }

    if (sb->items[sb->count - 1] != 0)
        nob_sb_append_null(sb);

    nob_read_entire_dir(sb->items, &children);
    if (sb->items[sb->count - 1] == 0)
        sb->count--;

    for (size_t i = 0; i < children.count; i++) {
        if (strcmp(children.items[i], ".") == 0)
            continue;
        if (strcmp(children.items[i], "..") == 0)
            continue;

        const size_t dir_qtd = sb->count;

        // nob_log(NOB_INFO, "Last Char: '%c'(count - 1)", sb->items[sb->count - 1]);
        if (sb->items[sb->count - 1] != '/') {
            // nob_log(NOB_INFO, "Appending '/' to sb");
            nob_sb_append_cstr(sb, "/");
        }

        nob_sb_append_cstr(sb, children.items[i]);
        nob_sb_append_null(sb);
        Nob_File_Type rst = nob_get_file_type(sb->items);
        // printf("File: %s | Type '%d'\n", sb->items, rst);
        char *s;
        switch (rst) {
        case NOB_FILE_DIRECTORY:
            sb->count--;
            recurse_dir(sb, eb);
            sb->count = dir_qtd;
            break;
        case NOB_FILE_REGULAR:
            // printf("%s\n",sb->items);
            s = strdup(sb->items);
            nob_da_append(eb, s);
            // sb->count--;
            sb->count = dir_qtd;
            break;
        default:
            continue;
        }
        // break;
    }
}

bool Bundler(char **path, size_t pathCount) {
    if (pathCount < 1 || path[0] == NULL)
        return true;

    Nob_String_Builder sb = { 0 };
    Nob_Procs procs = { 0 };
    EmbedFiles eb = { 0 };
    DIR *dir;
    nob_log(NOB_INFO, "Starting Dir read!");

    for (size_t i = 0; i < pathCount; i++) {
        sb.count = 0;
        dir = opendir(path[i]);
        if (dir == NULL) {
#ifdef _WIN32
            DWORD err = GetLastError();
            nob_log(NOB_ERROR, "Could not open directory '%s': %s (0x%X)", path[i], nob_win32_error_message(err), err);
#else
            nob_log(NOB_ERROR, "Could not open directory \"%s\": %s (0x%X)", path[i], strerror(errno), errno);
#endif // _WIN32

            // nob_log(NOB_ERROR, "Could not open directory '%s'", path[i]);
            return false;
        }
        closedir(dir);

        if (strncmp(path[i], "./", 2) != 0 && strncmp(path[i], ".\\", 2) != 0) {
            nob_sb_append_cstr(&sb, "./");
        }

        nob_sb_append_cstr(&sb, path[i]);

        recurse_dir(&sb, &eb);
    }
    if (eb.count) {
        nob_log(NOB_INFO, "Generating Resource Bundle");

        for (size_t i = 0; i < eb.count; i++) {
            Resource r = { .file_path = eb.items[i] };
            nob_da_append(&resources, r);
            nob_log(NOB_INFO, "    Adding \"%s\" to be bundled", resources.items[i].file_path);
        }
        if (!generate_resource_bundle())
            return false;
    } else {
        nob_log(NOB_ERROR, "No assets to bundle found...");
    }

    bool result = nob_procs_wait(procs);

    nob_sb_free(sb);
    nob_da_free(procs);
    nob_da_free(eb);

    return result;
}

bool CleanupFiles(void) {
    bool result = true;

    const bool binDir = nob_file_exists(BIN_DIR) > 0;
    const bool buildDir = nob_file_exists(BUILD_DIR) > 0;
    const bool wasmDir = nob_file_exists(WASM_DIR) > 0;
    const bool buildWasmDir = nob_file_exists(BUILD_WASM_DIR) > 0;

    const int totalDirs = binDir + buildDir + wasmDir + buildWasmDir;

    if (totalDirs == 0) {
        nob_log(NOB_INFO, "No files to be deleted, exiting...");
        return true;
    }

    Nob_String_Builder sb = { 0 };
    EmbedFiles eb = { 0 };

    if (buildDir) {
        sb.count = 0;
        nob_sb_append_cstr(&sb, BUILD_DIR);
        recurse_dir(&sb, &eb);
    }

    if (binDir) {
        sb.count = 0;
        nob_sb_append_cstr(&sb, BIN_DIR);
        recurse_dir(&sb, &eb);
    }

    if (wasmDir) {
        sb.count = 0;
        nob_sb_append_cstr(&sb, WASM_DIR);
        recurse_dir(&sb, &eb);
    }

    for (size_t i = 0; i < eb.count; i++) {
        // nob_log(NOB_INFO, "Removing file: '%s' (%zu/%zu)", eb.items[i], i + 1, eb.count);
        if (!nob_delete_file(eb.items[i])) {
            // nob_log(NOB_ERROR, "Could not remove file '%s': %s", eb.items[i], strerror(errno));
            nob_return_defer(false);
        }
    }

defer:
    nob_sb_free(sb);

    for (size_t i = 0; i < eb.count; i++)
        free(eb.items[i]);

    free(eb.items);

    fflush(stderr);

    return result;
}

bool TestFile(void) {
    bool result = true;

    Nob_Cmd cmd = { 0 };
    const char *input = "./outTest.c";
    const char *output = "./outTest.exe";

    int rebuild = nob_needs_rebuild1(output, input);
    if (rebuild < 0)
        nob_return_defer(false);

    if (rebuild > 0) {
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-o", output, input, "-Wall", "-Wextra", "-Og", "-g");

        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);

    } else {
        nob_log(NOB_INFO, "File \"%s\" is up to date", output);
    }

    nob_cmd_append(&cmd, output);
    nob_return_defer(nob_cmd_run_sync(cmd));

defer:
    nob_cmd_free(cmd);
    return result;
}

static volatile int pyWskeepRunning = 1;

void pyWsHandler(int dummy) {
    NOB_UNUSED(dummy);
    pyWskeepRunning = 0;
}

bool WebServer(const char *pyExec) {
#if !defined(_WIN32)
    NOB_TODO("Webserver handling not implemented for this platform");
    return false;
#else  // !defined(_WIN32)
    Nob_Cmd cmd = { 0 };
    bool result = true;

    if (nob_file_exists(WASM_DIR) != 1
        && (nob_file_exists(WASM_DIR PROGRAM_NAME ".html") != 1 || nob_file_exists(WASM_DIR "index.html") != 1)) {
        nob_log(NOB_ERROR, "Build wasm first");
        nob_return_defer(false);
    }

    if (nob_needs_rebuild1(WASM_DIR "index.html", WASM_DIR PROGRAM_NAME ".html") != 0) {
        if (!nob_copy_file(WASM_DIR PROGRAM_NAME ".html", WASM_DIR "index.html")) {
            nob_return_defer(false);
        };
    }

    nob_cmd_append(&cmd, pyExec, "-m", "http.server", "-d", WASM_DIR, "-b", "localhost", WS_PORT);

    Nob_Proc p = nob_cmd_run_async(cmd);
    if (p == NOB_INVALID_PROC)
        nob_return_defer(false);

    // Handling CTRL+C to close python server only
    signal(SIGINT, pyWsHandler);

    nob_log(NOB_INFO, "Press 'CTRL+C' to close the Web Server...");
    while (pyWskeepRunning) // infinite loop to interrupt main process
        (void)0;

    nob_log(NOB_INFO, "Closing Web Server...");

    if (!TerminateProcess(p, 0)) {
        nob_log(NOB_ERROR, "Could not terminate process \"%s\": %s", pyExec, nob_win32_error_message(GetLastError()));
        nob_return_defer(false);
    }

    if (!CloseHandle(p)) {
        nob_log(NOB_ERROR, "Could not close handle to process \"%s\": %s", pyExec, nob_win32_error_message(GetLastError()));
        nob_return_defer(false);
    }

    // Giving CTRL+C back to system
    signal(SIGINT, SIG_DFL);

    nob_log(NOB_INFO, "Web Server closed successfully");

defer:
    nob_cmd_free(cmd);

    return result;
#endif // _WIN32
}

#ifdef _WIN32
#define NULL_OUTPUT "NUL"
#else
#define NULL_OUTPUT "/dev/null"
#endif

bool TestCompiler(void) {
    bool result = true;
    Nob_Cmd cmd = { 0 };
    Nob_Fd nullOutput = nob_fd_open_for_write(NULL_OUTPUT);
    if (nullOutput == NOB_INVALID_FD) {
        nob_log(NOB_ERROR, "Could not open \"" NULL_OUTPUT "\" for dumping output");
        exit(1);
    }
    Nob_Cmd_Redirect cr = {
        .fdout = &nullOutput,
        .fderr = &nullOutput,
    };

#if defined(_MSC_VER)
    if (nob_file_exists(MSVC_ENV) != 1) {
        nob_log(NOB_ERROR, "Could not find MSVC env builder \"" MSVC_ENV "\", check if the path is correct and try again...");
        nob_return_defer(false);
    }
    MSVC(&cmd);
    nob_cmd_append(&cmd, CC);
#else
    nob_cmd_append(&cmd, CC, "-v");
#endif // !defined(_MSC_VER)

    const char *missingCompiler = "'%s' Compiler not found, aborting...";
    const char *foundCompiler = "Compiler '%s' Found";

    if (!nob_cmd_run_sync_redirect_and_reset(&cmd, cr)) {
        nob_log(NOB_ERROR, missingCompiler, CC);
        nob_return_defer(false);
    }
    nob_log(NOB_INFO, foundCompiler, CC);

    nob_cmd_append(&cmd, "c3c", "-V");
    if (!nob_cmd_run_sync_redirect_and_reset(&cmd, cr)) {
        nob_log(NOB_ERROR, missingCompiler, "c3c");
        nob_return_defer(false);
    }
    nob_log(NOB_INFO, foundCompiler, "c3c");

defer:
    nob_cmd_free(cmd);
    nob_fd_close(nullOutput);
    return result;
}

#ifdef _WIN32
#define C3_TARGET "--target", "mingw-x64"
#else
#define C3_TARGET ""
#endif

#define C3_TARGET_WEB "--target", "wasm32"

#ifdef RELEASE
#define C3_OPT "-O3", "-g0"
#else
#define C3_OPT "-O1", "-g"
#endif

bool C3Compile(bool isWeb) {
    const size_t temp_cp = nob_temp_save();
    bool result = true;

    const char *input = SRC_DIR "c3/c3teste.c3";

    // no extention, c3 compiler add automaticaly, Win32 = .obj, Linux = .o, etc...
    char *output = isWeb ? strdup(BUILD_WASM_DIR "c3teste") : strdup(BUILD_DIR "c3teste");

    const char *tmpOutput = isWeb ? nob_temp_sprintf("%s.o", output) : nob_temp_sprintf("%s.obj", output);

    Nob_Cmd cmd = { 0 };
    if (nob_needs_rebuild1(tmpOutput, input)) {
        // c3c.exe compile-only src/c3/c3teste.c3 -o build/c3teste --target mingw-x64 --single-module=yes -O1
        nob_cmd_append(&cmd, "c3c.exe", "compile-only", input, "-o", output, "--single-module=yes", C3_OPT, "-D", "NOMAIN");
        if (isWeb)
            nob_cmd_append(&cmd, C3_TARGET_WEB);
        else
            nob_cmd_append(&cmd, C3_TARGET);

        if (!nob_cmd_run_sync(cmd))
            nob_return_defer(false);

    } else {
        nob_log(NOB_INFO, skippingMsg, tmpOutput);
    }
    output = realloc(output, strlen(tmpOutput) + 1);
    strcpy(output, tmpOutput);
    nob_temp_rewind(temp_cp);

    // nob_log(NOB_INFO, "Output: '%s'", output);
    // exit(0);
defer:
    nob_da_append(&obj, output);
    nob_cmd_free(cmd);

    return result;
}
#if defined(_WIN32)
#define SHARED_USER_DATA (BYTE *)0x7FFE0000
WinVer GetWinVer(void) {
    WinVer result = {
        .major = *(ULONG *)(SHARED_USER_DATA + 0x26c), // major version offset
        .minor = *(ULONG *)(SHARED_USER_DATA + 0x270), // minor version offset
    };

    if (result.major >= 10UL)
        result.build = *(ULONG *)(SHARED_USER_DATA + 0x260); // build number offset

    return result;
}

// clang-format off

/*
    OLDER_WIN - Older Windows
    WIN_XP    - Windows XP
    WIN_VISTA - Windows Vista
    WIN_7     - Windows 7
    WIN_8     - Windows 8
    WIN_81    - Windows 8.1
    WIN_10    - Windows 10
    WIN_11    - Windows 11
*/
WVResp GetWindowsVersion(void) {
    const WinVer ver = GetWinVer();
    if (ver.build == 0) {
        if (ver.major < 5UL) return OLDER_WIN;
        if (ver.major < 6UL) return WIN_XP;
        if (ver.major == 6UL) {
            switch (ver.minor) {
                case 0UL: return WIN_VISTA;
                case 1UL: return WIN_7;
                case 2UL: return WIN_8;
                case 3UL: return WIN_81;
            }
        }
        NOB_UNREACHABLE("GetWinVer");
    }
    return ver.build >= 21996UL ? WIN_11 : WIN_10; // if build >= 21996 = Win 11 else Win 10
}
// clang-format on

#endif // defined(_WIN32)

bool ExeIsInPath(const char *exe) {
    Nob_Cmd cmd = { 0 };
    Nob_Cmd_Redirect cr = { 0 };

    Nob_Fd nullOutput = nob_fd_open_for_write(NULL_OUTPUT);
    Sleep(0);
    if (nullOutput == NOB_INVALID_FD) {
        nob_log(NOB_ERROR, "Could not dump output to \"" NULL_OUTPUT "\"");
        nob_log(NOB_ERROR, "Dumping to default outputs");
    } else {
        cr.fdout = &nullOutput;
        cr.fderr = &nullOutput;
    }
    nob_cmd_append(&cmd, FINDER, exe);
    bool result = nob_cmd_run_sync_redirect(cmd, cr);
    nob_cmd_free(cmd);
    if (result)
        nob_log(NOB_INFO, "\"%s\" found in PATH", exe);
    else
        nob_log(NOB_ERROR, "\"%s\" not found in PATH", exe);
    return result;
}
