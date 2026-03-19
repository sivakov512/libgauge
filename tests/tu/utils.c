#include "tu/utils.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void tu_ensure_parent_dir(const char *path) {
    char tmp[PATH_MAX];
    snprintf(tmp, sizeof(tmp), "%s", path);

    char *sep = strrchr(tmp, '/');
    if (!sep) {
        return;
    }
    *sep = '\0';

    for (char *seg = tmp + 1; *seg; seg++) {
        if (*seg == '/') {
            *seg = '\0';
            (void) mkdir(tmp, TU_DIR_MODE);
            *seg = '/';
        }
    }
    (void) mkdir(tmp, TU_DIR_MODE);
}
