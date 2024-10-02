#define SV_IMPLEMENTATION
#include "include/sv.h"

#define NOB_IMPLEMENTATION
#include "include/nob.h"

bool ParseJson(const char *input);
bool ParseJson2(const char *input);

bool ParseJson(const char *input) {
    bool result = true;
    char *content = NULL;
    FILE *f = fopen(input, "rb");

    if (f == NULL) {
        nob_log(NOB_ERROR, "Could not open file \"%s\": %s", input, strerror(errno));
        nob_return_defer(false);
    }

    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    rewind(f);

    if (fileSize <= 0)
        nob_return_defer(false);

    content = calloc(fileSize, 1);
    if (content == NULL)
        nob_return_defer(false);

    if (fread(content, 1, fileSize, f) != (size_t)fileSize)
        nob_return_defer(false);

    nob_log(NOB_INFO, "%.*s\n", fileSize, content);
    nob_log(NOB_INFO, "File Size: %zu bytes", fileSize);

defer:
    if (f)
        fclose(f);

    if (content)
        free(content);

    return result;
}

int main(void) {
    const char *input = "tests/00.json";

    // return !ParseJson(input);
    return !ParseJson2(input);
}

bool ParseJson2(const char *input) {
    bool result = true;

    FILE *f = fopen(input, "rt");
    if (f == NULL)
        return false;

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char *content = calloc(fsize, 1);
    fread(content, 1, fsize, f);
    fclose(f);

    String_View sv = sv_from_parts(content, fsize);
    sv_chop_left(&sv, 2);
    String_View line = sv_trim_left(sv_chop_by_delim(&sv, '\n'));
    // sv = nob_sv_chop_by_delim(&sv, '[');

    printf(SV_Fmt, SV_Arg(line));
    // printf(SV_Fmt, 10, line.data);
    printf("\n\n");
    printf("sv.count: %zu\n", sv.count);

    // free(sv.data);
    free(content);

    return result;
}
