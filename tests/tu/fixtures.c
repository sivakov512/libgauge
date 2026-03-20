#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/json.h"
#include <dirent.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

static bool is_jpeg(const char *name) {
    const char *dot = strrchr(name, '.');
    if (!dot) {
        return false;
    }
    return strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0;
}

static int cmp_name(const void *lhs, const void *rhs) {
    return strcmp((const char *) lhs, (const char *) rhs);
}

bool tu_fixtures_load_image(const char *fixtures_dir, const char *rel_path,
                            tu_image_t *out) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", fixtures_dir, rel_path);
    return tu_image_load(path, out);
}

size_t tu_fixtures_load_images(const char *fixtures_dir, const char *rel_dir,
                               tu_image_t *out, size_t max_count) {
    char dir_path[PATH_MAX];
    snprintf(dir_path, sizeof(dir_path), "%s/%s", fixtures_dir, rel_dir);

    DIR *dir = opendir(dir_path);
    if (!dir) {
        return 0;
    }

    char names[TU_MAX_FILES][NAME_MAX + 1];
    size_t found = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && found < TU_MAX_FILES) {
        if (entry->d_type != DT_REG) {
            continue;
        }
        if (!is_jpeg(entry->d_name)) {
            continue;
        }
        strncpy(names[found], entry->d_name, NAME_MAX);
        names[found][NAME_MAX] = '\0';
        found++;
    }

    qsort(names, found, sizeof(names[0]), cmp_name);

    size_t loaded = 0;
    for (size_t idx = 0; idx < found && loaded < max_count; idx++) {
        char file_path[PATH_MAX];
        snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, names[idx]);
        if (tu_image_load(file_path, &out[loaded])) {
            loaded++;
        }
    }

    closedir(dir);
    return loaded;
}

bool tu_fixtures_load_frame(const char *fixtures_dir, const char *rel_path,
                            gauge_frame_t *out) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", fixtures_dir, rel_path);
    return tu_json_read_frame(path, out);
}

bool tu_fixtures_load_line(const char *fixtures_dir, const char *rel_path,
                           gauge_line_t *out) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", fixtures_dir, rel_path);
    return tu_json_read_line(path, out);
}

bool tu_fixtures_load_calibration(const char *fixtures_dir, const char *rel_path,
                                  gauge_calibration_data_t *out) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", fixtures_dir, rel_path);
    return tu_json_read_calibration(path, out);
}
