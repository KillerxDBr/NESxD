#if defined(_WIN32)
#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_DECLARE_NONSTDC_NAMES 0
#endif

#include <locale.h>
#include <signal.h>

#include "build_src/shared.h"

#define FLAG_IMPLEMENTATION
#include "include/flag.h"

bool BuildRayLib(void);
bool CompileSingleHeaderLib(void);
bool CompileSourceFiles(void);
bool CompileDependencies(void);
bool CompileExecutable(void);
bool CompileImgui(void);

bool Bundler(char **path, size_t pathCount);
bool CleanupFiles(void);
bool CompileFiles(bool isWeb);
bool CompileSHLibs(bool isWeb);
bool IsExeInPath(const char *exe);
bool LinkExecutable(bool isWeb);
bool ParseDependencyFile(Nob_File_Paths *fp, Nob_String_Builder *sb, const char *depFile);
bool PrecompileHeader(bool isWeb);
bool TestCompiler(void);
bool TestFile(void);
bool WebServer(const char *pyExec);
bool delete_dir_recusive(const char *path);

#if defined(_WIN32) && 0
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

static WVResp GetWindowsVersion(void);
static WinVer GetWinVer(void);
#endif

const char *libs[] = {
#if defined(_WIN32)
    "gdi32", "winmm", "comdlg32", "ole32", "advapi32",
#else
    "m",
#endif // defined(_WIN32)
};

const char *files[] = {
    "main", "6502", "config", "gui", "input", "kxdMem", "iconTray", "lang",
#ifndef RELEASE
    "test",
#endif
};

const char *filesFlags[] = {
    DEF_FLAG "KXD_MAIN_FILE",  DEF_FLAG "KXD_6502_FILE", DEF_FLAG "KXD_CONFIG_FILE",   DEF_FLAG "KXD_GUI_FILE",
    DEF_FLAG "KXD_INPUT_FILE", DEF_FLAG "KXD_MEM_FILE",  DEF_FLAG "KXD_ICONTRAY_FILE", DEF_FLAG "KXD_LANG_FILE",
#ifndef RELEASE
    DEF_FLAG "KXD_TEST_FILE",
#endif
};

const char *dependencies[] = {
    "tinyfiledialogs",
#ifdef _WIN32
    "WindowsHeader",
#endif /* _WIN32 */
};

typedef struct {
    const char *path;
    const char *fileName;
    const char *implementation;
} SHLibs;

static const SHLibs shl[] = {
    {INC_DIR, "nob",    "NOB_IMPLEMENTATION"   },
    {INC_DIR, "raygui", "RAYGUI_IMPLEMENTATION"},
};

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

static Nob_Cmd cmd            = {};
static Resources resources    = {};
static Nob_File_Paths obj     = {};
static Nob_File_Paths defPath = {};

#define CCACHE "ccache"
bool hasCCache = false;

void PrintUsage(void) {
    nob_log(NOB_ERROR, "Incorrect use of program");
    nob_log(NOB_ERROR,
            "Usage: %s <[b]uild [[w]eb] [[c]lean], [w]eb [python executable], "
            "bundler [dir], [c]lean>",
            flag_program_name());
}

#if defined(_WIN32)
W32(int) SetConsoleOutputCP(uint32_t);
W32(int) PathResolve(wchar_t *pszPath, wchar_t **dirs, uint32_t fFlags);
#endif

const char *nob_header_path = "include/nob.h";
int main(int argc, char **argv) {
#if defined(_WIN32)
    setlocale(LC_CTYPE, ".UTF8");
    SetErrorMode(SEM_FAILCRITICALERRORS);

    if (!SetConsoleOutputCP(CP_UTF8)) {
        nob_log(NOB_WARNING, "Could not set console output to UTF-8");
    }

    int argc_bkp    = argc;
    char **argv_bkp = argv;
    if (!nob_win32_uft8_cmdline_args(&argc, &argv)) {
        nob_log(NOB_WARNING, "Could not generate Win32 UTF-8 Command Line Arguments, using default...");
        argc = argc_bkp;
        argv = argv_bkp;
    }
#endif

    NOB_GO_REBUILD_URSELF_PLUS(argc, argv,               //
                               nob_header_path,          //
                               "build_src/nob_imgui.c",  //
                               "build_src/nob_raylib.c", //
                               "build_src/shared.h");    //

    NOB_ASSERT(NOB_ARRAY_LEN(files) == NOB_ARRAY_LEN(filesFlags));

    int result = 0;

    bool *BuildMode   = flag_bool("b", false, "BuildMode");
    bool *BundlerMode = flag_bool("bundler", false, "BundlerMode");
    bool *CleanMode   = flag_bool("c", false, "CleanMode");
    bool *TestMode    = flag_bool("t", false, "TestMode");
    bool *WebMode     = flag_bool("w", false, "WebMode");
    bool *NoCCache    = flag_bool("ccache", false, "Disable " CCACHE " use");

    if (!flag_parse(argc, argv)) {
        PrintUsage();
        flag_print_error(stderr);
        exit(1);
    }

    if (!*BuildMode && !*BundlerMode && !*CleanMode && !*TestMode && !*WebMode) {
        *BuildMode = true;
    }

    if (*CleanMode) {
        bool cleanResult = delete_dir_recusive(BIN_DIR) && delete_dir_recusive(BLD_DIR);

        if (!cleanResult) {
            nob_log(NOB_ERROR, "Could not cleanup build artifacts");
            nob_return_defer(1);
        }
    }

    if (*BuildMode) {
        if (!nob_mkdir_if_not_exists(BIN_DIR))
            nob_return_defer(1);

        if (!nob_mkdir_if_not_exists(BLD_DIR))
            nob_return_defer(1);

        if (!*NoCCache)
            hasCCache = IsExeInPath(CCACHE);

        if (!BuildRayLib()) {
            nob_log(NOB_ERROR, "Could not build RayLib...");
            nob_return_defer(1);
        }

        if (!CompileSingleHeaderLib()) {
            nob_log(NOB_ERROR, "Could not Compile Single Header Libraries...");
            nob_return_defer(1);
        }

        if (!CompileSourceFiles()) {
            nob_log(NOB_ERROR, "Could not compile Source Files...");
            nob_return_defer(1);
        }

        if (!CompileDependencies()) {
            nob_log(NOB_ERROR, "Could not compile Dependencies...");
            nob_return_defer(1);
        }

        if (!CompileImgui()) {
            nob_log(NOB_ERROR, "Could not compile Imgui...");
            nob_return_defer(1);
        }

        if (!CompileExecutable()) {
            nob_log(NOB_ERROR, "Could not Link Executable...");
            nob_return_defer(1);
        }
    }

    if (*WebMode) {
        nob_log(NOB_WARNING, "%s Mode Not Implemented...", "WebMode");
    }

    if (*TestMode) {
        nob_log(NOB_WARNING, "%s Mode Not Implemented...", "TestMode");
    }

    if (*BundlerMode) {
        nob_log(NOB_WARNING, "%s Mode Not Implemented...", "BundlerMode");
    }

defer:
    for (size_t i = 0; i < obj.count; ++i) {
        free((void *)obj.items[i]);
    }
    nob_da_free(obj);

    for (size_t i = 0; i < defPath.count; ++i) {
        free((void *)defPath.items[i]);
    }
    nob_da_free(defPath);

    nob_da_free(resources);
    nob_cmd_free(cmd);

    return result;
}

#if defined(_WIN32) && 0
#define SHARED_USER_DATA (BYTE *)0x7FFE0000
static WinVer GetWinVer(void) {
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
static WVResp GetWindowsVersion(void) {
    const WinVer ver = GetWinVer();
    if (ver.build == 0) {
        if (ver.major < 5UL || (ver.major == 5UL && ver.minor < 1UL)) return OLDER_WIN;
        if (ver.major < 6UL) return WIN_XP;
        if (ver.major == 6UL) {
            switch (ver.minor) {
                case 0UL: return WIN_VISTA;
                case 1UL: return WIN_7;
                case 2UL: return WIN_8;
                case 3UL: return WIN_81;
            }
        }
        NOB_UNREACHABLE("GetWindowsVersion");
    }
    return ver.build >= 21996UL ? WIN_11 : WIN_10; // if build >= 21996 = Win 11 else Win 10
}
// clang-format on
#endif // defined(_WIN32)

#ifdef _WIN32
#define NULL_OUTPUT "NUL"
#define FINDER      "where"
#else
#define NULL_OUTPUT "/dev/null"
#define FINDER      "which"
#endif

bool IsExeInPath(const char *exe) {
    Nob_Cmd_Redirect cr = {};

    if (exe == NULL || *exe == '\0') {
        nob_log(NOB_ERROR, "Path is invalid");
        return false;
    }

    Nob_Fd nullOutput = nob_fd_open_for_write(NULL_OUTPUT);
    if (nullOutput == NOB_INVALID_FD) {
        nob_log(NOB_ERROR, "Could not dump output to \"" NULL_OUTPUT "\"");
        nob_log(NOB_ERROR, "Dumping to default outputs");
    } else {
        cr.fdout = &nullOutput;
        cr.fderr = &nullOutput;
    }

    nob_cmd_append(&cmd, FINDER, exe);
    bool result = nob_cmd_run_async_redirect_and_reset(&cmd, cr);

    if (result)
        nob_log(NOB_INFO, "\"%s\" found in PATH", exe);
    else
        nob_log(NOB_WARNING, "\"%s\" not found in PATH", exe);

    if (nullOutput != NOB_INVALID_FD)
        nob_fd_close(nullOutput);

    return result;
}

bool delete_dir_recusive(const char *path) {
    bool result           = true;
    Nob_File_Paths fp     = {};
    Nob_String_Builder sb = {};

    if (path == NULL || *path == '\0') {
        nob_log(NOB_ERROR, "Path is invalid");
        return false;
    }

    nob_log(NOB_INFO, "deleting '%s' recursively", path);

    if (nob_file_exists(path) < 1)
        nob_return_defer(true);

    Nob_File_Type type = nob_get_file_type(path);
    switch (type) {
    case NOB_FILE_REGULAR:
        nob_return_defer(nob_delete_file(path));
        break;
    case NOB_FILE_DIRECTORY:
        break;
    default:
        nob_log(NOB_ERROR, "Invalid file type for '%s'", path);
        nob_return_defer(false);
        break;
    }

    if (!nob_read_entire_dir(path, &fp)) {
        nob_return_defer(false);
    }

    nob_sb_append_cstr(&sb, path);

    if (sb.items[sb.count - 1] != '/' && sb.items[sb.count - 1] != '\\')
        nob_sb_append_cstr(&sb, "/");

    const size_t sb_cp = sb.count;

    for (size_t i = 0; i < fp.count; ++i) {
        if (strcmp(fp.items[i], ".") == 0)
            continue;
        if (strcmp(fp.items[i], "..") == 0)
            continue;

        sb.count = sb_cp;
        nob_sb_append_cstr(&sb, fp.items[i]);
        nob_sb_append_null(&sb);

        type = nob_get_file_type(sb.items);
        switch (type) {
        case NOB_FILE_REGULAR:
            if (!nob_delete_file(sb.items))
                nob_return_defer(false);
            break;
        case NOB_FILE_DIRECTORY:
            if (!delete_dir_recusive(sb.items))
                nob_return_defer(false);
            break;
        default:
            nob_log(NOB_WARNING, "Invalid file type for '%s', skipping...", sb.items);
            break;
        }
    }

    result = nob_delete_dir(path);

defer:
    nob_da_free(fp);
    nob_sb_free(sb);
    return result;
}

#define RAYLIB_SRC_PATH "extern/raylib/src/"

bool BuildRayLib(void) {
    // clang -c -o build\rcore.o .\extern\raylib\src\rcore.c -Wall -Wextra -MMD -DUSE_MINIRENT -D_CRT_SECURE_NO_WARNINGS
    // -----------------------------------------------------------------------------------------------------------------
    // clang -DGRAPHICS_API_OPENGL_33 -DPLATFORM_DESKTOP -D_CRT_SECURE_NO_WARNINGS -ID:/Projetos/NESxD/extern/raylib/src
    // -isystem D:/Projetos/NESxD/extern/raylib/src/external/glfw/include -fno-strict-aliasing
    // -Werror=implicit-function-declaration -Werror=pointer-arith  -O0 -std=gnu99 -D_DEBUG -D_DLL -D_MT -Xclang
    // --dependent-lib=msvcrtd -g -Xclang -gcodeview -MD -MT raylib/CMakeFiles/raylib.dir/raudio.c.obj -MF
    // raylib\CMakeFiles\raylib.dir\raudio.c.obj.d -o raylib/CMakeFiles/raylib.dir/raudio.c.obj -c
    // D:/Projetos/NESxD/extern/raylib/src/raudio.c

    // -DGRAPHICS_API_OPENGL_33 -DPLATFORM_DESKTOP -D_CRT_SECURE_NO_WARNINGS
    bool result = true;

    Nob_File_Paths fp       = {};
    Nob_String_Builder sb   = {};
    const size_t checkpoint = nob_temp_save();

    const char *rlFiles[] = {"rcore", "rglfw", "rshapes", "rtextures", "rtext", "utils", "rmodels", "raudio"};

    for (size_t i = 0; i < NOB_ARRAY_LEN(rlFiles); ++i) {
        const char *input   = nob_temp_sprintf("%s%s%s", RAYLIB_SRC_PATH, rlFiles[i], ".c");
        const char *output  = nob_temp_sprintf("%s%s%s", BLD_DIR, rlFiles[i], "_" CPL OBJ_EXT);
        const char *depFile = nob_temp_sprintf("%s%s%s", BLD_DIR, rlFiles[i], "_" CPL ".d");

        output = strdup(output);
        nob_da_append(&obj, output);

        if (!ParseDependencyFile(&fp, &sb, depFile)) {
            nob_da_append(&fp, input);
            nob_da_append(&fp, RAYLIB_SRC_PATH "config.h");
            char *header = nob_temp_strdup(input);
            char *SubStr = strstr(header, ".c");
            if (SubStr && *SubStr) {
                SubStr++;
                if (*SubStr == 'c') {
                    *SubStr = 'h';
                    if (nob_file_exists(header) > 0)
                        nob_da_append(&fp, header);
                }
            }
        }
#if defined(_MSC_VER) && !defined(__clang__)
        else {
            // need to add input for MSVC cl.exe, since the dependencies are from input file (.c/.cpp file)
            // when in GCC/Clang the dependencies are from object file (.o)
            nob_da_append(&fp, input);
        }
#endif

        if (nob_needs_rebuild(output, fp.items, fp.count) != 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);

            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);

            nob_cc(&cmd);

            nob_cc_include(&cmd, "./");
            nob_cc_include(&cmd, INC_DIR);
            nob_cc_include(&cmd, RAYLIB_SRC_PATH "external/glfw/include");
            nob_cmd_append(&cmd, NOLINK_FLAG);

            nob_cc_obj(&cmd, output);
            nob_cc_inputs(&cmd, input);
            // nob_cc_flags(&cmd);

#ifdef _WIN32
            nob_cmd_append(&cmd, DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE", //
                           DEF_FLAG "_CRT_SECURE_NO_WARNINGS", DEF_FLAG "USE_MINIRENT");
#endif

            nob_cc_depfile(&cmd, depfile);

#if !defined(_MSC_VER) || defined(__clang__)
            nob_cmd_append(&cmd, "-fno-strict-aliasing", "-Werror=implicit-function-declaration");
            nob_cmd_append(&cmd, "-Werror=pointer-arith", "-Wno-unused-function", "-Wno-unused-parameter");
#endif

            nob_cmd_append(&cmd, DEF_FLAG "GRAPHICS_API_OPENGL_33", DEF_FLAG "PLATFORM_DESKTOP");

            // sb.count = 0;
            // nob_cmd_render(cmd, &sb);
            // printf("CMD: " SV_Fmt "\n", (int)sb.count, sb.items);
            // continue;
#if defined(_MSC_VER) && !defined(__clang__)
            Nob_Fd outFile = nob_fd_open_for_write(depFile);
            if (outFile != NOB_INVALID_FD) {

                Nob_Cmd_Redirect red = {
                    .fdin  = NULL,     //
                    .fdout = &outFile, //
                    .fderr = NULL,     //
                };

                if (!nob_cmd_run_sync_redirect_and_reset(&cmd, red))
                    nob_return_defer(false);

                sb.count = 0;
                if (nob_read_entire_file(depFile, &sb)) {
                    size_t buffSize = 0;
                    char *buff      = nob_temp_alloc(sb.count);

                    NOB_ASSERT(buff && "Increase NOB_TEMP_CAPACITY");

                    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));
                    while (sv.count) {
                        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
                        if (nob_sv_starts_with(sv2, msvc_prefix)) {
                            memcpy(&buff[buffSize], sv2.data, sv2.count);
                            buffSize += sv2.count;

                            memset(&buff[buffSize], '\n', 1);
                            buffSize++;
                        } else {
                            fprintf(stderr, SV_Fmt "\n", SV_Arg(sv2));
                        }
                    }
                    fflush(stderr);
                    nob_write_entire_file(depFile, buff, buffSize);
                } else {
                    nob_log(NOB_WARNING, "Could not print compiler output to console, check '%s' for the entire output",
                            depFile);
                }
            } else {
                nob_log(NOB_WARNING, "Could not redirect compiler output");
                if (!nob_cmd_run_sync_and_reset(&cmd))
                    nob_return_defer(false);
            }
#else
            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
#endif
        }
    }

defer:
    nob_temp_rewind(checkpoint);
    return result;
}

#if !defined(__clang__) && defined(_MSC_VER)

W32(BOOL) PathIsRelativeW(_In_ LPCWSTR pszPath);

bool get_MSVC_default_paths(void) {
    DWORD sz = GetEnvironmentVariableW(L"INCLUDE", NULL, 0);
    if (!sz) {
        nob_log(NOB_ERROR, "Could not find Environment Variable: %s", nob_win32_error_message(GetLastError()));
        return false;
    }

    char *path     = nob_temp_alloc(MAX_PATH);
    wchar_t *wPath = nob_temp_alloc(MAX_PATH * sizeof(wchar_t));
    wchar_t *wEnv  = nob_temp_alloc(sz * sizeof(wchar_t));
    NOB_ASSERT(wEnv && wPath && path && "Increase NOB_TEMP_CAPACITY");

    if (!GetEnvironmentVariableW(L"INCLUDE", wEnv, sz)) {
        nob_log(NOB_ERROR, "Could not find Environment Variable: %s", nob_win32_error_message(GetLastError()));
        return false;
    }

    wchar_t *cp = wEnv;

    for (wchar_t *wc = wEnv; wc <= wEnv + sz; ++wc) {
        if (wc == cp)
            continue;
        if (*wc == L';' || *wc == '\0') {
            *wc = 0;

            memcpy(wPath, cp, (char *)wc - (char *)cp + 2);
            if (!PathResolve(wPath, NULL, 1)) {
                nob_log(NOB_ERROR, "PathResolve failed: %s", nob_win32_error_message(GetLastError()));
                return false;
            }

            if (!WideCharToMultiByte(CP_UTF8, 0, wPath, -1, path, MAX_PATH, NULL, NULL)) {
                nob_log(NOB_ERROR, "WideCharToMultiByte failed: %s", nob_win32_error_message(GetLastError()));
                return false;
            }

            char *tmp = strdup(path);
            nob_da_append(&defPath, tmp);

            cp = wc + 1;
        }
    }
    return true;
}
#endif // #if !defined(__clang__) && defined(_MSC_VER)

bool ParseDependencyFile(Nob_File_Paths *fp, Nob_String_Builder *sb, const char *depFile) {
    NOB_ASSERT(fp);
    NOB_ASSERT(sb);

    fp->count = 0;
    sb->count = 0;

    if (!depFile || !*depFile)
        return false;

    if (nob_file_exists(depFile) < 1)
        return false;

    if (!nob_read_entire_file(depFile, sb))
        return false;

    Nob_String_View sv;

#if !defined(__clang__) && defined(_MSC_VER)
    static bool populate = true;

    if (!defPath.count && populate) {
        if (!get_MSVC_default_paths()) {
            nob_log(NOB_WARNING, "Could not find MSVC include paths, system headers will be checked for changes...");
            populate = false;
        }
    }

    sv = nob_sv_trim(nob_sb_to_sv(*sb));

    char *path        = nob_temp_alloc(MAX_PATH);
    wchar_t *wPath    = nob_temp_alloc(MAX_PATH * sizeof(wchar_t));
    wchar_t *wTmpPath = nob_temp_alloc(MAX_PATH * sizeof(wchar_t));

    NOB_ASSERT(path && wPath && wTmpPath && "Increase NOB_TEMP_CAPACITY");

    while (sv.count) {
        nob_sv_chop_by_sv(&sv, msvc_prefix);
        if (!sv.count)
            break;

        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        if (!sv2.count)
            continue;

        int rst;
        rst = snprintf(path, MAX_PATH, SV_Fmt, SV_Arg(sv2));
        if (rst < 1 || rst >= MAX_PATH) {
            nob_log(NOB_ERROR, "snprintf failed: %s", strerror(errno ? errno : ENOBUFS));
            continue;
        }

        for (size_t i = 0; i < MAX_PATH; ++i)
            if (path[i] == '/')
                path[i] = '\\';

        if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wPath, MAX_PATH)) {
            nob_log(NOB_ERROR, "Failed to convert '%s' to Wide String: %s", path,
                    nob_win32_error_message(GetLastError()));
            continue;
        }

        // if (wPath[1] == L':' && wPath[2] == L'\\') {
        if (!PathIsRelativeW(wPath)) {
            rst = PathResolve(wPath, NULL, 1);
        } else {
            rst = GetFullPathNameW(wPath, MAX_PATH, wTmpPath, NULL);
            memcpy(wPath, wTmpPath, (sizeof(wchar_t) * MAX_PATH));
        }

        if (!rst || rst > MAX_PATH) {
            nob_log(NOB_ERROR, "Failed to get full path for '%s': %s", path, nob_win32_error_message(GetLastError()));
            continue;
        }

        if (!WideCharToMultiByte(CP_UTF8, 0, wPath, -1, path, MAX_PATH, NULL, NULL)) {
            nob_log(NOB_ERROR, "Failed to convert '%s' to Narrow String: %s", path,
                    nob_win32_error_message(GetLastError()));
            continue;
        }

        bool isSystemHeader = false;
        for (size_t i = 0; i < defPath.count; ++i) {
            isSystemHeader = nob_sv_starts_with(nob_sv_from_cstr(path), nob_sv_from_cstr(defPath.items[i]));
            if (isSystemHeader)
                break;
        }

        if (!isSystemHeader) {
            char *cstr = nob_temp_strdup(path);
            nob_da_append(fp, cstr);
        }

        sv = nob_sv_trim(sv);
    }
#else
    sv = nob_sv_trim(nob_sb_to_sv(*sb));

    if (sv.count > 2 && isalpha(sv.data[0]) && sv.data[1] == ':' && sv.data[2] == '\\') {
        // remove ':' from C:\\ on win32, will give problems if
        // first file name is one letter only (dont know if its possible)
        nob_sv_chop_by_delim(&sv, ':');
    }

    nob_sv_chop_by_delim(&sv, ':');

    sv = nob_sv_trim(sv);

    while (sv.count) {
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, ' '));
        if (!sv2.count)
            continue;
        if (!nob_sv_eq(sv2, SV("\\"))) {
            // nob_log(NOB_INFO, "%s[%zu]: \"" SV_Fmt "\"", depFile, fp->count + 1, SV_Arg(sv2));
            char *cstr = nob_temp_sprintf(SV_Fmt, SV_Arg(sv2));
            nob_da_append(fp, cstr);
        }
        sv = nob_sv_trim(sv);
    }
#endif
    return fp->count > 0;
    // #endif
}

bool CompileSingleHeaderLib(void) {
    const size_t checkpoint = nob_temp_save();
    bool result             = true;

    for (size_t i = 0; i < NOB_ARRAY_LEN(shl); ++i) {
        const char *input  = nob_temp_sprintf("%s%s%s", shl[i].path, shl[i].fileName, ".h");
        const char *output = nob_temp_sprintf("%s%s%s", BLD_DIR, shl[i].fileName, "_" CPL OBJ_EXT);
        const char *impl   = nob_temp_sprintf("%s%s", DEF_FLAG, shl[i].implementation);

        output = strdup(output);
        nob_da_append(&obj, output);

        if (nob_needs_rebuild1(output, input) != 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);

            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);

            nob_cc(&cmd);

            nob_cc_output_obj(&cmd, output);

            nob_cmd_append(&cmd, C_MODE);
            nob_cc_inputs(&cmd, input);

            nob_cc_flags(&cmd);

#ifdef _WIN32
            nob_cmd_append(&cmd, DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE", DEF_FLAG "_CRT_SECURE_NO_WARNINGS");
#endif

            nob_cmd_append(&cmd, impl, DEF_FLAG "NRPUB", NOLINK_FLAG);

            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
        }
    }

defer:
    nob_temp_rewind(checkpoint);
    return result;
}

bool CompileSourceFiles(void) {
    const size_t checkpoint = nob_temp_save();
    bool result             = true;

    Nob_File_Paths fp     = {};
    Nob_String_Builder sb = {};

    for (size_t i = 0; i < NOB_ARRAY_LEN(files); ++i) {
        const char *input   = nob_temp_sprintf("%s%s%s", SRC_DIR, files[i], ".c");
        const char *output  = nob_temp_sprintf("%s%s%s", BLD_DIR, files[i], "_" CPL OBJ_EXT);
        const char *depFile = nob_temp_sprintf("%s%s%s", BLD_DIR, files[i], "_" CPL ".d");

        output = strdup(output);
        nob_da_append(&obj, output);

        if (!ParseDependencyFile(&fp, &sb, depFile)) {
            nob_da_append(&fp, input);
            char *header = nob_temp_strdup(input);
            char *SubStr = strstr(header, ".c");
            if (SubStr && *SubStr) {
                SubStr++;
                if (*SubStr == 'c') {
                    *SubStr = 'h';
                    if (nob_file_exists(header) > 0)
                        nob_da_append(&fp, header);
                }
            }
        }
#if defined(_MSC_VER) && !defined(__clang__)
        else {
            // need to add input for MSVC cl.exe, since the dependencies are from input file (.c/.cpp file)
            // when in GCC/Clang the dependencies are from object file (.o)
            nob_da_append(&fp, input);
        }
#endif

        if (nob_needs_rebuild(output, fp.items, fp.count) != 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);

            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);

            nob_cc(&cmd);

            nob_cc_include(&cmd, "./");
            nob_cc_include(&cmd, INC_DIR);
            nob_cc_include(&cmd, EXT_DIR);
            nob_cc_include(&cmd, "./styles/");

            nob_cc_depfile(&cmd, depfile);

            nob_cc_output_obj(&cmd, output);
            nob_cc_inputs(&cmd, input);
            nob_cc_flags(&cmd);

#ifdef _WIN32
            nob_cmd_append(&cmd, DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE", DEF_FLAG "_CRT_SECURE_NO_WARNINGS");
#endif

            nob_cmd_append(&cmd, filesFlags[i], NOLINK_FLAG, DEF_FLAG "KXD_DEBUG");
#if defined(_MSC_VER) && !defined(__clang__)
            Nob_Fd outFile = nob_fd_open_for_write(depFile);
            if (outFile != NOB_INVALID_FD) {

                Nob_Cmd_Redirect red = {
                    .fdin  = NULL,     //
                    .fdout = &outFile, //
                    .fderr = NULL,     //
                };

                if (!nob_cmd_run_sync_redirect_and_reset(&cmd, red))
                    nob_return_defer(false);

                sb.count = 0;
                if (nob_read_entire_file(depFile, &sb)) {
                    size_t buffSize = 0;
                    char *buff      = nob_temp_alloc(sb.count);

                    NOB_ASSERT(buff && "Increase NOB_TEMP_CAPACITY");

                    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));
                    while (sv.count) {
                        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
                        if (nob_sv_starts_with(sv2, msvc_prefix)) {
                            memcpy(&buff[buffSize], sv2.data, sv2.count);
                            buffSize += sv2.count;

                            memset(&buff[buffSize], '\n', 1);
                            buffSize++;
                        } else {
                            fprintf(stderr, SV_Fmt "\n", SV_Arg(sv2));
                        }
                    }
                    fflush(stderr);
                    nob_write_entire_file(depFile, buff, buffSize);
                } else {
                    nob_log(NOB_WARNING, "Could not print compiler output to console, check '%s' for the entire output",
                            depFile);
                }
            } else {
                nob_log(NOB_WARNING, "Could not redirect compiler output");
                if (!nob_cmd_run_sync_and_reset(&cmd))
                    nob_return_defer(false);
            }
#else
            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
#endif
        }
    }

#if defined(_WIN32)
    const char *rc_input = "resource.rc";
    const char *manifest = "manifest.xml";
    if (nob_file_exists(rc_input) > 0 && nob_file_exists(manifest) > 0) {
        const char *resource = BLD_DIR "resource_"CPL".res";
        const char *inputs[] = {rc_input, manifest};

        if (nob_needs_rebuild(resource, inputs, 2) != 0) {
            nob_log(NOB_INFO, "--- Building %s file", resource);
            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);

            nob_res(&cmd, rc_input, resource);

            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
        }
        resource = strdup(resource);
        nob_da_append(&obj, resource);
    }
#endif

defer:
    nob_da_free(fp);
    nob_sb_free(sb);

    nob_temp_rewind(checkpoint);

    return result;
}

bool CompileDependencies(void) {
    bool result             = true;
    const size_t checkpoint = nob_temp_save();

    for (size_t i = 0; i < NOB_ARRAY_LEN(dependencies); ++i) {
        const char *input  = nob_temp_sprintf(EXT_DIR "%s/%s%s", dependencies[i], dependencies[i], ".c");
        const char *output = nob_temp_sprintf("%s%s%s", BLD_DIR, dependencies[i], "_" CPL OBJ_EXT);

        output = strdup(output);
        nob_da_append(&obj, output);

        if (nob_needs_rebuild1(output, input) != 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);

            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);

            nob_cc(&cmd);

            nob_cc_include(&cmd, "./");
            nob_cc_include(&cmd, INC_DIR);

            nob_cc_output_obj(&cmd, output);
            nob_cc_inputs(&cmd, input);

            nob_cc_flags(&cmd);

#ifdef _WIN32
            nob_cmd_append(&cmd, DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE", DEF_FLAG "_CRT_SECURE_NO_WARNINGS");
#endif

            nob_cmd_append(&cmd, NOLINK_FLAG);

            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
        }
    }

defer:
    nob_temp_rewind(checkpoint);
    return result;
}

bool CompileImgui(void) {
    bool result             = true;
    const size_t checkpoint = nob_temp_save();

    const char *imgui[] = {
        "cimgui/cimgui",              //
        "cimgui/imgui/imgui",         //
        "cimgui/imgui/imgui_demo",    //
        "cimgui/imgui/imgui_draw",    //
        "cimgui/imgui/imgui_tables",  //
        "cimgui/imgui/imgui_widgets", //
        "rlImGui/rlImGui",            //
    };

    for (size_t i = 0; i < NOB_ARRAY_LEN(imgui); ++i) {
        const char *input  = nob_temp_sprintf("%s%s%s", EXT_DIR, imgui[i], ".cpp");
        const char *output = nob_temp_sprintf("%s%s%s", BLD_DIR, nob_path_name(imgui[i]), "_" CPL OBJ_EXT);

        output = strdup(output);
        nob_da_append(&obj, output);

        if (nob_needs_rebuild1(output, input) != 0) {
            nob_log(NOB_INFO, "--- Generating %s ---", output);

            if (hasCCache)
                nob_cmd_append(&cmd, CCACHE);

            nob_cxx(&cmd);

            nob_cc_include(&cmd, "./");
            nob_cc_include(&cmd, INC_DIR);
            nob_cc_include(&cmd, EXT_DIR "rlImGui/");
            nob_cc_include(&cmd, EXT_DIR "cimgui/");
            nob_cc_include(&cmd, EXT_DIR "cimgui/imgui/");

            nob_cc_output_obj(&cmd, output);
            nob_cc_inputs(&cmd, input);

            // nob_cc_flags(&cmd);

#ifdef _WIN32
            nob_cmd_append(&cmd, DEF_FLAG "UNICODE", DEF_FLAG "_UNICODE", DEF_FLAG "_CRT_SECURE_NO_WARNINGS");
#endif

            nob_cmd_append(&cmd, DEF_FLAG "CIMGUI_NO_EXPORT", NOLINK_FLAG);

            if (!nob_cmd_run_sync_and_reset(&cmd))
                nob_return_defer(false);
        }
    }

defer:
    nob_temp_rewind(checkpoint);
    return result;
}

bool CompileExecutable(void) {
    bool result = true;

    const char *output = BIN_DIR EXENAME;

    if (nob_needs_rebuild(output, obj.items, obj.count) != 0) {
        nob_log(NOB_INFO, "--- Generating %s ---", output);

        if (hasCCache)
            nob_cmd_append(&cmd, CCACHE);

        nob_cc(&cmd);

        nob_cc_output(&cmd, output);
        nob_da_append_many(&cmd, obj.items, obj.count);

        nob_cc_flags(&cmd);

        for (size_t i = 0; i < NOB_ARRAY_LEN(libs); ++i) {
            const char *lib = nob_lib(libs[i]);
            nob_cmd_append(&cmd, lib);
        }

        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    }

defer:
    return result;
}