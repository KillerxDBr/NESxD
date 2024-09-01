#include <stdio.h>
#include <string.h>

#define NOB_IMPLEMENTATION
#include <nob.h>

#define genf(out, ...)                                                                                                                     \
    do {                                                                                                                                   \
        fprintf((out), __VA_ARGS__);                                                                                                       \
        fprintf((out), " // %s:%d\n", __FILE__, __LINE__);                                                                                 \
    } while (0)

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

Resources resources;

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
        genf(out, "");
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

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} EmbedFiles;

bool recurse_dir(Nob_String_Builder *sb, EmbedFiles *eb) {
    bool result = true;
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

        // printf("sb.count: %zu\n", dir_qtd);

        nob_sb_append_cstr(sb, "/");
        nob_sb_append_cstr(sb, children.items[i]);
        nob_sb_append_null(sb);
        Nob_File_Type rst = nob_get_file_type(sb->items);
        printf("File: %s | Type '%d'\n", sb->items, rst);
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

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    // const char *program = nob_shift_args(&argc, &argv);

    Nob_Procs procs = { 0 };
    procs.count = 0;

    nob_log(NOB_INFO, "Starting Dir read!");
    // Nob_File_Paths children = {0};
    // Nob_String_View tmpPath = {0};
    Nob_String_Builder sb = { 0 };

    const char *path = "./rom";
    nob_sb_append_cstr(&sb, path);

    EmbedFiles eb = { 0 };
    bool rst = recurse_dir(&sb, &eb);
    // printf("EB count: %zu\n", eb.count);

    if (eb.count) {
        nob_log(NOB_INFO, "Generating Resource Bundle");

        for (size_t i = 0; i < eb.count; i++) {
            Resource r = { .file_path = eb.items[i] };
            nob_da_append(&resources, r);
            nob_log(NOB_INFO, "    Adding \"%s\" to be bundled", resources.items[i].file_path);
        }
        if (!generate_resource_bundle())
            return 1;
    } else {
        nob_log(NOB_ERROR, "No assets to bundle found...");
    }

    if (!nob_procs_wait(procs))
        return 1;

    return 0;
}
