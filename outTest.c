#define NOB_IMPLEMENTATION
#include "include/nob.h"

bool IsExeInPath(const char *exe);

#define LOG_INF(...)                                                                                                                       \
    do {                                                                                                                                   \
        fprintf(stdout, __VA_ARGS__);                                                                                                      \
        fflush(stdout);                                                                                                                    \
    } while (0)
#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#define VARLOG(v, fmt) fprintf(stdout, "%s: " fmt, (#v), (v))
#define BOOLLOG(v) fprintf(stdout, "%s: %s", #v, v ? "True" : "False")

static inline void *callocWrapperFunc(size_t n, size_t sz, const char *fileName, const int lineNum) {
    void *ptr = calloc(n, sz);
    if (ptr == NULL) {
        LOG_ERR("%s:%d:  Could not allocate memory (%zu bytes), exiting...", fileName, lineNum, n * sz);
        // fprintf(stderr, "%s:%d: ERROR: Could not allocate memory (%zu bytes), exiting...\n\n", fileName, lineNum, n * sz);
        fflush(stderr); abort();
    }
    return ptr;
}
// void *callocWrapper(size_t _NumOfElements, Type)
#define callocWrapper(n, T) ((T *)callocWrapperFunc((n), sizeof(T), (__FILE__), (__LINE__)))

bool ParseJson(const char *input);

#if 1
int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, nob_header_path());
    IsExeInPath("gcc");
    return 0;
#else
int main(void) {
#endif
#ifdef _WIN32
    const BYTE *sharedUserData = (BYTE *)0x7FFE0000;
    printf("Windows Version: %lu.%lu.%lu\n",
           *(ULONG *)(sharedUserData + 0x26c),  // major version offset
           *(ULONG *)(sharedUserData + 0x270),  // minor version offset
           *(ULONG *)(sharedUserData + 0x260)); // build number offset
#endif
    return 0;
    const char *input = "tests/00.json";

    return !ParseJson(input);
}

bool ParseJson(const char *input) {
    bool result = true;
    char *content = NULL;
    FILE *f = fopen(input, "rb");

    if (f == NULL) {
        nob_log(NOB_ERROR, "Could not open file \"%s\": %s", input, strerror(errno));
        nob_return_defer(false);
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        nob_log(NOB_ERROR, "Could not change file pointer position to end");
        nob_return_defer(false);
    }

    long fileSize = ftell(f);

    if (fileSize == -1) {
        nob_log(NOB_ERROR, "Could not read file \"%s\" size: %s", input, strerror(errno));
        nob_return_defer(false);
    }

    if (fseek(f, 0, SEEK_SET) != 0) {
        nob_log(NOB_ERROR, "Could not change file pointer position to start");
        nob_return_defer(false);
    }

    content = callocWrapper(fileSize, char);

    if (fread(content, 1, fileSize, f) != (size_t)fileSize) {
        if (feof(f)) {
            nob_log(NOB_ERROR, "Unexpected end of file \"%s\"", input);
        } else if (ferror(f)) {
            nob_log(NOB_ERROR, "Could not read file \"%s\"", input);
        }
        nob_return_defer(false);
    }

    printf("%.*s\n", 50, content);
    nob_log(NOB_INFO, "File Size: %zu bytes", fileSize);

defer:
    if (f)
        fclose(f);

    if (content)
        free(content);

    return result;
}

#ifdef _WIN32
#define NULL_OUTPUT "NUL"
#define FINDER "where"
#else
#define NULL_OUTPUT "/dev/null"
#define FINDER "which"
#endif

bool IsExeInPath(const char *exe) {
    Nob_Cmd cmd = { 0 };
    Nob_Fd nullOutput = nob_fd_open_for_write(NULL_OUTPUT);
    if (nullOutput == NOB_INVALID_FD) {
        nob_log(NOB_ERROR, "Could not dump output to \"" NULL_OUTPUT "\"");
        nob_log(NOB_ERROR, "Dumping to default outputs");
    }
    Nob_Cmd_Redirect cr = {
        .fdout = &nullOutput,
        .fderr = &nullOutput,
    };
    nob_cmd_append(&cmd, FINDER, exe);
    bool result = nob_cmd_run_sync_redirect(cmd, cr);
    nob_cmd_free(cmd);
    if (result)
        nob_log(NOB_INFO, "\"%s\" found in PATH", exe);
    else
        nob_log(NOB_WARNING, "\"%s\" not found in PATH", exe);
    return result;
}
