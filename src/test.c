#include "test.h"

bool InstructionTest(app_t *app) {
    if (!nob_mkdir_if_not_exists(TEST_DIR))
        return false;
    Nob_File_Paths fp = { 0 };
    // read test files
    if (!nob_read_entire_dir(TEST_DIR, &fp))
        return false;

    LOG_INF("fp.count: %zu", fp.count);
    if (fp.count < 2)
        return false;
    
    for (size_t i = 2; i < fp.count; i++) {
        LOG_INF("%s", fp.items[i]);
    }

    return true;
}
