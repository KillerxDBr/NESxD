#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif

// #define RELEASE

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool PrecompileHeader(void);
bool CompileFiles(void);
bool CleanupFiles(void);
bool CompileDependencies(void);
bool CompileExecutable(void);
bool CompileNobHeader(void);
bool Bundler(void);

#define NOB_IMPLEMENTATION
#include "include/nob.h"

#if _WIN32
#if defined(__GNUC__)
#define CC "gcc"
#else
#error No GCC Found...
/*
#elif defined(__clang__)
#define CC "clang"
#elif defined(_MSC_VER)
#error MSVC UNSUPORTED
#define CC "cl"
#endif
#else
#define CC "cc"
*/

#endif
#endif

#define RAYLIB "./lib/libraylib.a"

#define BUILD_DIR "build"
#define BIN_DIR "bin"
#define SRC_DIR "src"
#define EXTERN_DIR "extern"

#define EXE_NAME "nesxd.exe"

#define EXE_OUTPUT BIN_DIR "/" EXE_NAME

#define PCH_SUFFIX "_pch.h"
#define GCH_SUFFIX "_pch.h.gch"

#define NOB_H_DIR "include/nob.h"

#define INCLUDES "-Ibuild/", "-I.", "-Iinclude", "-Isrc", "-Istyles", "-Iextern"

#define LIBS "-lgdi32", "-lwinmm", "-lcomdlg32", "-lole32"

const char *files[] = {
    "main", "6502", "config", "gui", "input",
};
#define FILES_COUNT sizeof(files) / sizeof(files[0])

const char *dependencies[] = {
    "tinyfiledialogs",
};

#define DEPENDENCIES_COUNT sizeof(dependencies) / sizeof(dependencies[0])

const char *skippingMsg = "    File '%s' already up to date, skipping...";

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} Objects;

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

void GetIncludedHeaders(Objects *eh, const char *header);

bool ccache;

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    // const char *program = nob_shift_args(&argc, &argv);
    // (void)program;
    nob_shift_args(&argc, &argv);

    Nob_Cmd cmd = { 0 };
    nob_cmd_append(&cmd, "where", "ccache");

    ccache = nob_cmd_run_sync(cmd);

    nob_cmd_free(cmd);

    nob_log(NOB_INFO, "Using Compiler: \"%s\"", CC);

    const char *command;
    if (argc <= 0)
        command = "build";
    else
        command = nob_shift_args(&argc, &argv);

    if (strcmp(command, "build") == 0) {
        nob_log(NOB_INFO, "--- Building ---");
        if (argc > 0) {
            if (strcmp(command, "clean") == 0 || strcmp(command, "cls") == 0) {
                nob_log(NOB_INFO, "    Forcing Rebuild of all files...");
                if (!CleanupFiles())
                    return 1;
            }
        }

        if (!nob_mkdir_if_not_exists(BUILD_DIR))
            return 1;

        nob_log(NOB_INFO, "--- Building Dependencies ---");
        if (!CompileDependencies())
            return 1;

        nob_log(NOB_INFO, "--- Precompiling Headers ---");
        if (!PrecompileHeader())
            return 1;

        nob_log(NOB_INFO, "--- Generating Object Files ---");
        if (!CompileFiles())
            return 1;

        nob_log(NOB_INFO, "--- Compiling Executable ---");

        if (!nob_mkdir_if_not_exists(BIN_DIR))
            return 1;

        if (!CompileExecutable())
            return 1;

        nob_log(NOB_INFO, "--- Finished Compiling ---");

    } else if (strcmp(command, "bundler") == 0) {
        nob_log(NOB_INFO, "--- Bundler ---");
        if (!Bundler())
            return 1;
        // assert(0 && "Not Implemented!");

    } else if (strcmp(command, "clean") == 0 || strcmp(command, "cls") == 0) {
        nob_log(NOB_INFO, "--- Cleaning Files ---");
        if (!CleanupFiles())
            return 1;

    } else {
        nob_log(NOB_INFO, "Unknown command, expects: <build, bundler, clean/cls>");
    }

    for (size_t i = 0; i < obj.count; i++) {
        free((void *)obj.items[i]);
    }
    nob_da_free(obj);

    return 0;
}

// clang-format off
#ifndef RELEASE
// CFlags
#define CFLAGS                                                                                          \
        "-Wall",                                                                                        \
        "-Wextra",                                                                                      \
        "-Winvalid-pch",                                                                                \
        "-std=gnu11",                                                                                   \
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
        "-std=gnu11",                                                                                   \
        "-O3",                                                                                          \
        "-march=native",                                                                                \
        "-DPLATFORM_DESKTOP",                                                                           \
        "-DGRAPHICS_API_OPENGL_33",                                                                     \
        "-DNES",                                                                                        \
        "-mwindows"
#endif
// clang-format on

#define STR_SIZE 64ULL

bool PrecompileHeader(void) {
    Objects extraHeaders = { 0 };
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    if (ccache) {
        nob_cmd_append(&cmd, "ccache");
    }
    nob_cmd_append(&cmd, CC, "-fdiagnostics-color=always", "-xc-header", CFLAGS, INCLUDES);
    size_t command_size = cmd.count;

    /* ccache gcc -fdiagnostics-color=always -xc-header -Ibuild/
    -I. -Iinclude -Isrc -Istyles -Iextern -o build/6502precomp.h.gch
    src/6502.h -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb -march=native
    -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
    */
    //-o build/6502precomp.h.gch src/6502.h
    Nob_String_Builder sb = { 0 };
    for (size_t i = 0; i < FILES_COUNT; i++) {
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

        GetIncludedHeaders(&extraHeaders, input);
        nob_da_append(&extraHeaders, input);

        nob_log(NOB_INFO, "------------------------------------");
        for (size_t j = 0; j < extraHeaders.count; j++)
        {
            nob_log(NOB_INFO, "%s", extraHeaders.items[j]);
        }
        nob_log(NOB_INFO, "------------------------------------");
        

        if (nob_needs_rebuild(output, extraHeaders.items, extraHeaders.count)) {
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

bool CompileExecutable(void) {
    Nob_Cmd cmd = { 0 };
    if (ccache) {
        nob_cmd_append(&cmd, "ccache");
    }

    // for (size_t i = 0; i < obj.count; i++) {
    //     nob_log(NOB_INFO, "%zu: %s", i + 1ULL, obj.items[i]);
    // }
    // exit(0);
    size_t cnt = cmd.count;
    // gcc -o bin/nesxd.exe build/6502.o build/config.o build/gui.o build/input.o build/main.o build/nob.o build/resource.o
    // build/tinyfiledialogs.o lib/libraylib.a -lgdi32 -lwinmm -lcomdlg32 -lole32
    if (nob_needs_rebuild(EXE_OUTPUT, obj.items, obj.count)) {
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=always");
        nob_cmd_append(&cmd, "-o", EXE_OUTPUT);
        nob_da_append_many(&cmd, obj.items, obj.count);
        nob_cmd_append(&cmd, RAYLIB, CFLAGS, LIBS);
    } else
        nob_log(NOB_INFO, skippingMsg, EXE_OUTPUT);

    bool result = cmd.count > cnt ? nob_cmd_run_sync(cmd) : true;
    nob_cmd_free(cmd);

    return result;
}

bool CompileFiles(void) {
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };

    if (ccache) {
        nob_cmd_append(&cmd, "ccache");
    }
    nob_cmd_append(&cmd, CC, "-fdiagnostics-color=always", "-xc", CFLAGS, INCLUDES);
    size_t command_size = cmd.count;

    /*
    ccache gcc -fdiagnostics-color=always -include build/6502precomp.h
    -Ibuild/ -I. -Iinclude -Isrc -Istyles -Iextern -o build/6502.o
    -c src/6502.c -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb
    -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -DNES
    */

    Nob_String_Builder sb = { 0 };
    for (size_t i = 0; i < FILES_COUNT; i++) {
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
        // char *toObj = malloc(strlen(output) + 1);
        // strcpy(toObj, output);
        // nob_da_append(&obj, toObj);
        nob_da_append(&obj, strdup(output));
    }
    // nob_log(NOB_INFO, "------------------");

    // windres -i resource.rc -o build/resource.o

    const char *rc_input = "resource.rc";
    const char *manifest = "manifest.xml";
    if (nob_file_exists(rc_input) == 1 && nob_file_exists(manifest) == 1) {
        const char *resource = BUILD_DIR "/resource.o";
        const char *inputs[] = { rc_input, manifest };

        if (nob_needs_rebuild(resource, inputs, 2)) {
            nob_log(NOB_INFO, "    Building %s file", rc_input);
            cmd.count = 0;
            nob_cmd_append(&cmd, "windres", "-i", rc_input, "-o", resource);
            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, resource);
        }
        // char *toObj = malloc(strlen(resource) + 1);
        // strcpy(toObj, resource);
        // nob_da_append(&obj, toObj);
        nob_da_append(&obj, strdup(resource));
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
    Nob_String_Builder sb = { 0 };
    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };
    if (ccache) {
        nob_cmd_append(&cmd, "ccache");
    }
    /*
    gcc -fdiagnostics-color=always -Ibuild/ -I. -Iinclude -Isrc -Istyles -Iextern -o bin/nesxd.exe build/main.o build/6502.o build/input.o
    build/config.o build/gui.o build/nob.o build/tinyfiledialogs.o build/resource.o -Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb
    -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 ./lib/libraylib.a -lgdi32 -lwinmm -lcomdlg32
    -lole32 -DKXD_DEBUG*/
    nob_cmd_append(&cmd, CC, "-fdiagnostics-color=always", "-xc", CFLAGS, INCLUDES);

    for (size_t i = 0; i < DEPENDENCIES_COUNT; ++i) {
        char input[STR_SIZE] = { 0 };
        char output[STR_SIZE] = { 0 };

        // input
        sb.count = 0;
        nob_sb_append_cstr(&sb, EXTERN_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, dependencies[i]);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, dependencies[i]);
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
        nob_sb_append_cstr(&sb, dependencies[i]);
        nob_sb_append_cstr(&sb, ".o");
        nob_sb_append_null(&sb);

        if (sb.count > STR_SIZE) {
            goto defer;
        }

        strcpy(output, sb.items);

        // nob_log(NOB_INFO, "Input: %s", input);
        // nob_log(NOB_INFO, "Output: %s", output);

        if (nob_needs_rebuild1(output, input)) {
            nob_log(NOB_INFO, "Rebuilding '%s' file", output);
            nob_cmd_append(&cmd, "-o", output, "-c", input);
            nob_da_append(&procs, nob_cmd_run_async(cmd));
        } else {
            nob_log(NOB_INFO, skippingMsg, output);
        }
        nob_da_append(&obj, strdup(output));
    }

    bool result = procs.count > 0 ? nob_procs_wait(procs) : true;
    nob_cmd_free(cmd);
    NOB_FREE(procs.items);
    nob_sb_free(sb);
    return CompileNobHeader() & result;
defer:
    nob_log(NOB_ERROR, "String Buffer is too small, size: %d, expects: %zu", STR_SIZE, sb.count);
    return false;
}

bool CompileNobHeader(void) {
    Nob_Cmd cmd = { 0 };
    if (ccache) {
        nob_cmd_append(&cmd, "ccache");
    }
    const char *output = BUILD_DIR "/nob.o";

    size_t cnt = cmd.count;
    if (nob_needs_rebuild1(output, NOB_H_DIR)) {
        nob_log(NOB_INFO, "Rebuilding '%s' file", output);
        nob_cmd_append(&cmd, CC, "-fdiagnostics-color=always", "-xc", "-o", output, "-c", NOB_H_DIR, "-DNOB_IMPLEMENTATION");
    } else {
        nob_log(NOB_INFO, skippingMsg, output);
    }

    // nob_da_append(&obj, output);
    nob_da_append(&obj, strdup(output));

    bool result = cmd.count > cnt ? nob_cmd_run_sync(cmd) : true;
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
    free(content.items);
    free(bundle.items);
    return result;
}

void recurse_dir(Nob_String_Builder *sb, EmbedFiles *eb) {
    Nob_File_Paths children = { 0 };

    if (sb->items[sb->count] != 0)
        nob_sb_append_null(sb);

    nob_read_entire_dir(sb->items, &children);
    if (sb->items[sb->count - 1] == 0)
        sb->count--;

    for (size_t i = 0; i < children.count; i++) {
        if (strcmp(children.items[i], ".") == 0)
            continue;
        if (strcmp(children.items[i], "..") == 0)
            continue;

        size_t dir_qtd = sb->count;

        nob_sb_append_cstr(sb, "/");
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

bool Bundler(void) {
    // assert(0 && "Not Implemented!");

    Nob_String_Builder sb = { 0 };
    Nob_Procs procs = { 0 };
    nob_log(NOB_INFO, "Starting Dir read!");

    const char *path = "./rom";
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

    Nob_String_Builder sb = { 0 };
    nob_sb_append_cstr(&sb, "./" BUILD_DIR);

    EmbedFiles eb = { 0 };
    recurse_dir(&sb, &eb);

    sb.count = 0;
    nob_sb_append_cstr(&sb, "./" BIN_DIR);
    recurse_dir(&sb, &eb);

    nob_sb_free(sb);

    for (size_t i = 0; i < eb.count; i++) {
        nob_log(NOB_INFO, "Removing file: '%s' (%zu/%zu)", eb.items[i], i + 1, eb.count);
        if (remove(eb.items[i]) != 0) {
            nob_log(NOB_ERROR, "Could not remove file '%s': %s", eb.items[i], strerror(errno));
            return false;
        }
    }
    bool binDir = nob_file_exists(BIN_DIR);
    bool buildDir = nob_file_exists(BUILD_DIR);
    int totalDirs = binDir + buildDir;
    int dirCount = 1;

    if (binDir) {
        nob_log(NOB_INFO, "Removing directory: '%s' (%d/%d)", BIN_DIR, dirCount, totalDirs);
        if (rmdir(BIN_DIR) != 0)
            nob_log(NOB_ERROR, "Could not remove directory '%s': %s", BIN_DIR, strerror(errno));
        dirCount++;
    }

    if (buildDir) {
        nob_log(NOB_INFO, "Removing directory: '%s' (%d/%d)", BUILD_DIR, dirCount, totalDirs);
        if (rmdir(BUILD_DIR) != 0)
            nob_log(NOB_ERROR, "Could not remove directory '%s': %s", BUILD_DIR, strerror(errno));
        dirCount++;
    }

    for (size_t i = 0; i < eb.count; i++)
        free(eb.items[i]);

    free(eb.items);
    return result;
}

#define INCLUDE_TXT "#include \""

void GetIncludedHeaders(Objects *eh, const char *header) {
    Nob_String_Builder sb = {0};
    eh->count = 0;
    char line[1024] = { 0 };

    FILE *fHeader = fopen(header, "rt");

    if (fHeader == NULL)
        return;
    size_t sz;
    nob_log(NOB_INFO, "Header: '%s'", header);
    while (feof(fHeader) == 0) {
        line[0] = '\0';
        fgets(line, sizeof(line), fHeader);
        sz = strlen(line);
        if (sz <= 1)
            continue;
        line[--sz] = '\0'; // removing \n from end

        if (strncmp(line, INCLUDE_TXT, sizeof(INCLUDE_TXT) - 1) == 0) {
            // char *hName = strdup(line) + sizeof(INCLUDE_TXT) - 1;
            // hName[strlen(hName) - 1] = '\0'; // removing last " from line

            sb.count = 0;
            nob_sb_append_buf(&sb, line + sizeof(INCLUDE_TXT) - 1, sz + sizeof(INCLUDE_TXT) - 2);
            nob_sb_append_null(&sb);
            nob_log(NOB_INFO, "%s", sb.items);
            
            // nob_da_append(eh, hName);
        }
    }
    exit(0);
    nob_log(NOB_INFO, "%zu Headers found in %s", eh->count, header);
    for (size_t i = 0; i < eh->count; ++i) {
        nob_log(NOB_INFO, "   %s", eh->items[i]);
    }
}
