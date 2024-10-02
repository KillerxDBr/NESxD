#define NOB_IMPLEMENTATION
#include "include/nob.h"

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

    return !ParseJson(input);
}
