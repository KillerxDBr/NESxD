#define SV_IMPLEMENTATION
#include "include/sv.h"

#define NOB_IMPLEMENTATION
#include "include/nob.h"

#include "src/kxdDebug.h"

#undef NOB_GO_REBUILD_URSELF
#define NOB_GO_REBUILD_URSELF(argc, argv)                                                                                                  \
    do {                                                                                                                                   \
        const char *source_path = __FILE__;                                                                                                \
        assert(argc >= 1);                                                                                                                 \
        const char *binary_path = argv[0];                                                                                                 \
                                                                                                                                           \
        int rebuild_is_needed = nob_needs_rebuild(binary_path, &source_path, 1);                                                           \
        if (rebuild_is_needed < 0)                                                                                                         \
            exit(1);                                                                                                                       \
        if (rebuild_is_needed) {                                                                                                           \
            Nob_String_Builder sb = { 0 };                                                                                                 \
            nob_sb_append_cstr(&sb, binary_path);                                                                                          \
            nob_sb_append_cstr(&sb, ".old");                                                                                               \
            nob_sb_append_null(&sb);                                                                                                       \
                                                                                                                                           \
            if (!nob_rename(binary_path, sb.items))                                                                                        \
                exit(1);                                                                                                                   \
            Nob_Cmd rebuild = { 0 };                                                                                                       \
            nob_cmd_append(&rebuild, NOB_REBUILD_URSELF(binary_path, source_path), "-O3");                                                 \
            bool rebuild_succeeded = nob_cmd_run_sync(rebuild);                                                                            \
            nob_cmd_free(rebuild);                                                                                                         \
            if (!rebuild_succeeded) {                                                                                                      \
                nob_rename(sb.items, binary_path);                                                                                         \
                exit(1);                                                                                                                   \
            }                                                                                                                              \
                                                                                                                                           \
            Nob_Cmd cmd = { 0 };                                                                                                           \
            nob_da_append_many(&cmd, argv, argc);                                                                                          \
            if (!nob_cmd_run_sync(cmd))                                                                                                    \
                exit(1);                                                                                                                   \
            exit(0);                                                                                                                       \
        }                                                                                                                                  \
    } while (0)

bool ParseJson(const char *input);

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

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

    content = callocWrapper(fileSize, 1);

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
