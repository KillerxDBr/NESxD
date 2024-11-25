#define NOB_IMPLEMENTATION
#include "include/nob.h"

#include <locale.h>
#include <signal.h>

// #define RELEASE
// #define USE_SDL
#define STATIC
#define ROM_PATH "rom/"
#define MEM_BIN_PATH "mem.bin"

/*
#if defined(_WIN32)
#define EMSDK_ENV "D:/emsdk/emsdk_env.bat"

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
*/

#define WS_PORT "8000"

#if defined(__GNUC__)
#define CC "gcc"
#elif defined(__clang__)
#define CC "clang"
#elif defined(_MSC_VER)
#error Cant Compile with MSVC yet...
#define CC "cl"
#define MSVC_ENV "D:/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat"

#define MSVC(cmd)                                                                                                                          \
    do {                                                                                                                                   \
        nob_cmd_append((cmd), "cmd", "/c");                                                                                                \
        nob_cmd_append((cmd), MSVC_ENV);                                                                                                   \
        nob_cmd_append((cmd), "&&");                                                                                                       \
    } while (0)
#else
#define CC "cc"
#endif // defined(__GNUC__)

#if defined(_MSC_VER)
#define NO_LINK_FLAG "/c"
#else
#define NO_LINK_FLAG "-c"
#endif // defined(_MSC_VER)

#define EMCC "emcc"

#define RAYLIB_A "./lib/libraylib.a"

#define BUILD_DIR "build/"
#define WASM_DIR "wasm/"
#define BUILD_WASM_DIR BUILD_DIR WASM_DIR
#define BIN_DIR "bin/"
#define SRC_DIR "src/"
#define LIB_DIR "lib/"
#define EXTERN_DIR "extern/"

#define PROGRAM_NAME "nesxd"

#if defined(_WIN32)
#include <VersionHelpers.h>

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

#define INCLUDES "-I.", "-I" BUILD_DIR, "-Iinclude", "-I" SRC_DIR, "-Istyles", "-I" EXTERN_DIR

#if defined(_WIN32)
const char *libs[] = { "gdi32", "winmm", "comdlg32", "ole32" };
#else
#define LIBS "-lm"
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

const char *files[] = {
    "main", "6502", "config", "gui", "input", "kxdMem",
#ifndef RELEASE
    "test",
#endif
};

const char *filesFlags[] = {
    "-DKXD_MAIN_FILE", "-DKXD_6502_FILE", "-DKXD_CONFIG_FILE", "-DKXD_GUI_FILE", "-DKXD_INPUT_FILE", "-DKXD_MEM_FILE",
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

const char *dirs[] = { BUILD_DIR, BIN_DIR, SRC_DIR, EXTERN_DIR };

const char *skippingMsg = "    File '%s' already up to date, skipping...";

#if !defined(STATIC)
const char *libraries[] = {
    //  "cimgui",
};
#endif

typedef Nob_File_Paths Objects;

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

Objects obj = { 0 };
Resources resources = { 0 };

bool buildRayLib(bool isWeb);
bool PrecompileHeader(bool isWeb);
bool CompileFiles(bool isWeb);
bool CleanupFiles(void);
bool CompileDependencies(bool isWeb);
bool CompileExecutable(bool isWeb);
bool CompileNobHeader(bool isWeb);
bool Bundler(const char *path);
bool GetIncludedHeaders(Objects *eh, const char *header);
bool TestFile(void);
bool WebServer(const char *pyExec);
void errorTest(void);
bool TestCompiler(void);

#ifdef STATIC
bool staticCompile = true;
#else
bool staticCompile = false;
#endif

#include "build_src/nob_raylib.c"

void PrintUsage(void) {
    nob_log(NOB_ERROR, "Incorrect use of program");
    nob_log(NOB_ERROR, "Usage: ./nob <[b]uild [[w]eb] [[c]lean/cls], [w]eb [python executable], bundler [dir], [c]lean/cls>");
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

#if defined(_WIN32) // Should be Win10+ only, but methods to detect windows versions are unreliable...
    nob_log(NOB_INFO, "Enabling buffer on console std outputs");
    // on Windows 10+ we need buffering or console will get 1 byte at a time (screwing up utf-8 encoding)
    if (setvbuf(stderr, NULL, _IOFBF, 1024) != 0) {
        nob_log(NOB_ERROR, "Could not set \"%s\" buffer to %d: %s", "stderr", 1024, strerror(errno));
        return 1;
    }

    if (setvbuf(stdout, NULL, _IOFBF, 1024) != 0) {
        nob_log(NOB_ERROR, "Could not set \"%s\" buffer to %d: %s", "stdout", 1024, strerror(errno));
        return 1;
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

    const char *program = nob_shift_args(&argc, &argv);
    NOB_UNUSED(program);
    // nob_shift_args(&argc, &argv);

    nob_log(NOB_INFO, "Using Compiler: \"%s\"", CC);
    nob_log(NOB_INFO, "Using WASM Compiler: \"%s\"", EMCC);

    const char *command;
    if (argc <= 0)
        command = "build";
    else
        command = nob_shift_args(&argc, &argv);

    int result = 0;

    if (strcmp(command, "build") == 0 || strcmp(command, "b") == 0) {
        nob_log(NOB_INFO, "--- Building ---");

        bool isWeb = false;

        if (!nob_mkdir_if_not_exists(BUILD_DIR))
            nob_return_defer(1);

        nob_log(NOB_INFO, "Testing Compiler: '" CC "'");
        if (!TestCompiler()) {
            nob_log(NOB_ERROR, "Could not find compiler '" CC "' in PATH");
            nob_return_defer(1);
        }

        if (argc > 0) {
            command = nob_shift_args(&argc, &argv);
            // nob.exe build clean/cls
            if ((strcmp(command, "clean") == 0) || (strcmp(command, "cls") == 0) || (strcmp(command, "c") == 0)) {
                nob_log(NOB_INFO, "    Forcing Rebuild of all files...");
                CleanupFiles();
            } else if (strcmp(command, "web") == 0 || strcmp(command, "w") == 0) {
                if (!nob_mkdir_if_not_exists(BUILD_WASM_DIR))
                    nob_return_defer(1);

                isWeb = true;
                if (argc > 0) {
                    command = nob_shift_args(&argc, &argv);
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

        const size_t bkpCount = obj.count;
        nob_log(NOB_INFO, "--- Building Raylib ---");
        if (!buildRayLib(isWeb)) {
            if (nob_file_exists(RAYLIB_A) != 1)
                nob_return_defer(1);

            obj.count = bkpCount;
            nob_da_append(&obj, RAYLIB_A);
        }

        nob_log(NOB_INFO, "--- Building Dependencies ---");
        if (!CompileDependencies(isWeb))
            nob_return_defer(1);

        if (!isWeb) {
            nob_log(NOB_INFO, "--- Precompiling Headers ---");
            if (!PrecompileHeader(false))
                nob_return_defer(1);
        }

        nob_log(NOB_INFO, "--- Generating Object Files ---");
        if (!CompileFiles(isWeb))
            nob_return_defer(1);

        nob_log(NOB_INFO, "--- Compiling Executable ---");

        if (!nob_mkdir_if_not_exists(BIN_DIR))
            nob_return_defer(1);

        if (isWeb) {
            if (!nob_mkdir_if_not_exists(WASM_DIR))
                nob_return_defer(1);
        }

        if (!CompileExecutable(isWeb))
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

        const char *bundlerPath;
        if (argc > 0)
            bundlerPath = nob_shift_args(&argc, &argv);
        else
            bundlerPath = NULL;

        nob_return_defer(!Bundler(bundlerPath));

    } else if ((strcmp(command, "test") == 0) || (strcmp(command, "t") == 0)) {
        nob_log(NOB_INFO, "--- Building Test File ---");
        nob_return_defer(!TestFile());

    } else if ((strcmp(command, "web") == 0) || (strcmp(command, "w") == 0)) {
        nob_log(NOB_INFO, "--- Starting WebServer ---");

        if (argc > 0)
            command = nob_shift_args(&argc, &argv);
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
        "-ggdb3",                                                                                       \
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
#define WFLAGS  "-Os",                                                                   \
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
                "-sASSERTIONS=1"

// clang-format on

// #define STR_SIZE 128

bool PrecompileHeader(bool isWeb) {
    if (isWeb)
        return true;
    Objects extraHeaders = { 0 };
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    if (isWeb) {
        EMS(&cmd);
        nob_cmd_append(&cmd, EMCC, "-fdiagnostics-color=never", "-xc-header", WFLAGS);
    } else {
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-xc-header", CFLAGS);
    }

    nob_cmd_append(&cmd, INCLUDES);
    const size_t command_size = cmd.count;

    /* ccache gcc -fdiagnostics-color=always -xc-header -Ibuild/
    -I. -Iinclude -Isrc -Istyles -Iextern -o build/6502precomp.h.gch
    src/6502.h -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb -march=native
    -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
    */
    //-o build/6502precomp.h.gch src/6502.h
    Nob_String_Builder sb = { 0 };
    for (size_t i = 0; i < NOB_ARRAY_LEN(files); i++) {
        size_t internalCheckpoint = nob_temp_save();

        cmd.count = command_size;
        extraHeaders.count = 0;

        char *input;
        char *output;

        // input file
        sb.count = 0;
        nob_sb_append_cstr(&sb, SRC_DIR);
        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, ".h");
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(input, sb.items);

        input = nob_temp_strdup(sb.items);

        // output
        sb.count = 0;

        if (isWeb)
            nob_sb_append_cstr(&sb, BUILD_WASM_DIR);
        else
            nob_sb_append_cstr(&sb, BUILD_DIR);

        nob_sb_append_cstr(&sb, files[i]);
        nob_sb_append_cstr(&sb, GCH_SUFFIX);
        nob_sb_append_null(&sb);

        // if (sb.count > STR_SIZE) {
        //     goto defer;
        // }

        // strcpy(output, sb.items);

        output = nob_temp_strdup(sb.items);

        if (GetIncludedHeaders(&extraHeaders, input)) {
            nob_da_append(&extraHeaders, input);
        }

        // nob_log(NOB_INFO, "------------------------------------");
        // for (size_t j = 0; j < extraHeaders.count; j++) {
        //     nob_log(NOB_INFO, "Extra Headers: %s", extraHeaders.items[j]);
        // }
        // nob_log(NOB_INFO, "Number of extra headers: %zu", extraHeaders.count);
        // nob_log(NOB_INFO, "------------------------------------");

        if (nob_needs_rebuild(output, extraHeaders.items, extraHeaders.count) != 0) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-o", output, input, filesFlags[i]);

            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }

        nob_temp_rewind(internalCheckpoint);
    }
    nob_cmd_free(cmd);
    nob_sb_free(sb);

    bool result = nob_procs_wait(procs);
    nob_da_free(procs);

    return result;

    // defer:
    //     nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    //     return false;
}

bool CompileExecutable(bool isWeb) {
    const size_t checkpoint = nob_temp_save();
    bool result = true;
    Nob_Cmd cmd = { 0 };

    for (size_t i = 0; i < obj.count; ++i) {
        if (nob_file_exists(obj.items[i]) != 1) {
            nob_log(NOB_ERROR, "Could not find file '%s', cleaning all files and aborting compilation...", obj.items[i]);
            CleanupFiles();
            nob_return_defer(false);
        }
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
            nob_cmd_append(&cmd, strdup(sb.items));
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
        Nob_String_Builder sb_libs = { 0 };
        for (size_t i = 0; i < NOB_ARRAY_LEN(libs); i++)
#if defined(_MSC_VER)
        {
            sb_libs.count = 0;
            nob_sb_append_cstr(&sb_libs, libs[i]);
            nob_sb_append_cstr(&sb_libs, ".lib");
            nob_sb_append_null(&sb_libs);
            nob_cmd_append(&cmd, strdup(sb_libs.items));
        }
#else  // defined(_MSC_VER)
        {
            sb_libs.count = 0;
            nob_sb_append_cstr(&sb_libs, "-l");
            nob_sb_append_cstr(&sb_libs, libs[i]);
            nob_sb_append_null(&sb_libs);
            nob_cmd_append(&cmd, strdup(sb_libs.items));
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
        nob_da_append(&obj, strdup(output));

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
                nob_cmd_append(&cmd, "windres", "-i", rc_input, "-o", resource);
                nob_da_append(&procs, nob_cmd_run_async(cmd));
            } else {
                nob_log(NOB_INFO, skippingMsg, resource);
            }
            nob_da_append(&obj, strdup(resource));
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
        nob_da_append(&obj, strdup(output));

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
            nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-xc", "-o", output, NO_LINK_FLAG, nob_header_dir,
                           "-DNOB_IMPLEMENTATION");

            if (!nob_cmd_run_sync(cmd))
                nob_return_defer(false);
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
    }
    nob_da_append(&obj, strdup(output));

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
        if (sb->items[sb->count - 1] != '\\' && sb->items[sb->count - 1] != '/') {
            // nob_log(NOB_INFO, "Appending '/' to sb");
            nob_sb_append_cstr(sb, "/");
        }

        nob_sb_append_cstr(sb, children.items[i]);
        nob_sb_append_null(sb);
        Nob_File_Type rst = nob_get_file_type(sb->items);
        // printf("File: %s | Type '%d'\n", sb->items, rst);
        switch (rst) {
        case NOB_FILE_DIRECTORY:
            sb->count--;
            recurse_dir(sb, eb);
            sb->count = dir_qtd;
            break;
        case NOB_FILE_REGULAR:
            // printf("%s\n",sb->items);
            nob_da_append(eb, strdup(sb->items));
            // sb->count--;
            sb->count = dir_qtd;
            break;
        default:
            continue;
        }
        // break;
    }
}

bool Bundler(const char *path) {
    if (path == NULL)
        path = ROM_PATH;

    Nob_String_Builder sb = { 0 };
    Nob_Procs procs = { 0 };
    nob_log(NOB_INFO, "Starting Dir read!");

    DIR *dir = NULL;
    dir = opendir(path);
    if (dir == NULL) {
#ifdef _WIN32
        DWORD err = GetLastError();
        nob_log(NOB_ERROR, "Could not open directory '%s': %s (0x%X)", path, nob_win32_error_message(err), err);
#else
        nob_log(NOB_ERROR, "Could not open directory \"%s\": %s (0x%X)", path, strerror(errno), errno);
#endif // _WIN32

        // nob_log(NOB_ERROR, "Could not open directory '%s'", path);
        return false;
    }
    closedir(dir);

    nob_sb_append_cstr(&sb, path);

    EmbedFiles eb = { 0 };
    recurse_dir(&sb, &eb);
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
        nob_log(NOB_INFO, "Removing file: '%s' (%zu/%zu)", eb.items[i], i + 1, eb.count);
        if (remove(eb.items[i]) != 0) {
            nob_log(NOB_ERROR, "Could not remove file '%s': %s", eb.items[i], strerror(errno));
            nob_return_defer(false);
        }
    }

    int dirCount = 1;

    if (binDir) {
        nob_log(NOB_INFO, "Removing directory: '%s' (%d/%d)", BIN_DIR, dirCount, totalDirs);
        if (rmdir(BIN_DIR) != 0)
            nob_log(NOB_ERROR, "Could not remove directory '%s': %s", BIN_DIR, strerror(errno));
        dirCount++;
    }

    if (wasmDir) {
        nob_log(NOB_INFO, "Removing directory: '%s' (%d/%d)", WASM_DIR, dirCount, totalDirs);
        if (rmdir(WASM_DIR) != 0)
            nob_log(NOB_ERROR, "Could not remove directory '%s': %s", WASM_DIR, strerror(errno));
        dirCount++;
    }

    if (buildWasmDir) {
        nob_log(NOB_INFO, "Removing directory: '%s' (%d/%d)", BUILD_WASM_DIR, dirCount, totalDirs);
        if (rmdir(BUILD_WASM_DIR) != 0)
            nob_log(NOB_ERROR, "Could not remove directory '%s': %s", BUILD_WASM_DIR, strerror(errno));
        dirCount++;
    }

    if (buildDir) {
        nob_log(NOB_INFO, "Removing directory: '%s' (%d/%d)", BUILD_DIR, dirCount, totalDirs);
        if (rmdir(BUILD_DIR) != 0)
            nob_log(NOB_ERROR, "Could not remove directory '%s': %s", BUILD_DIR, strerror(errno));
        dirCount++;
    }

defer:
    nob_sb_free(sb);

    for (size_t i = 0; i < eb.count; i++)
        free(eb.items[i]);

    free(eb.items);

    fflush(stderr);

    return result;
}

#define INCLUDE_TXT "#include \""
#define INC_TXT_SZ (sizeof(INCLUDE_TXT) - 1)

bool GetIncludedHeaders(Objects *eh, const char *header) {
    bool result = true;

    Nob_String_Builder sb = { 0 };
    char line[1024] = { 0 };

    FILE *fHeader = fopen(header, "rt");

    if (fHeader == NULL) {
        nob_log(NOB_ERROR, "Could not open header file '%s': %s", header, strerror(errno));
        nob_return_defer(false);
    }

    size_t sz;
    // nob_log(NOB_INFO, "Header: '%s'", header);
    while (feof(fHeader) == 0) {
        fgets(line, sizeof(line), fHeader);
        sz = strlen(line);
        if (sz < INC_TXT_SZ)
            continue;
        line[--sz] = '\0'; // removing \n from end

        if (strncmp(line, INCLUDE_TXT, INC_TXT_SZ) == 0) {
            for (size_t i = 0; i < NOB_ARRAY_LEN(dirs); ++i) {
                sb.count = 0;
                // nob_sb_append_cstr(&sb, "./");
                nob_sb_append_cstr(&sb, dirs[i]);
                nob_sb_append_buf(&sb, line + INC_TXT_SZ, sz - sizeof(INCLUDE_TXT));

                nob_sb_append_null(&sb);

                if (nob_file_exists(sb.items) == 1) {
                    // nob_log(NOB_INFO, "  File Exists: %s", sb.items);
                    nob_da_append(eh, strdup(sb.items));
                }
            }
        }
    }

    for (size_t i = 0; i < eh->count; ++i) {
        if (strcmp(eh->items[i], "src/InsFlags.h") == 0) {
            nob_da_append(eh, "src/instructions.h");
            break;
        }
    }

defer:
    nob_sb_free(sb);

    if (fHeader)
        fclose(fHeader);

    return result;
}

bool TestFile(void) {
    const char *input = "outTest.c";
    const char *output = "outTest.exe";
    if (nob_needs_rebuild1(output, input) != 0) {
        Nob_Cmd cmd = { 0 };
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=never", "-o", output, input, "-Wall", "-Wextra", "-O2");
        return nob_cmd_run_sync(cmd);
    }
    nob_log(NOB_INFO, "File \"%s\" is up to date", output);
    return true;
}

static volatile int pyWskeepRunning = 1;

void pyWsHandler(int dummy) {
    NOB_UNUSED(dummy);
    pyWskeepRunning = 0;
}

bool WebServer(const char *pyExec) {
#if !defined(_WIN32)
    NOB_TODO("Webserver handling not implemented for this platform");
    nob_return_defer(false);
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

    if (!nob_cmd_run_sync_redirect(cmd, (Nob_Cmd_Redirect){ .fdout = &nullOutput, .fderr = &nullOutput }))
        nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    nob_fd_close(nullOutput);
    return result;
}
