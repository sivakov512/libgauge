#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/* TEST_FIXTURES_DIR — input data dumps */
/* TEST_SNAPSHOTS_DIR — expected output dumps for assertions */
/* TEST_EXAMPLES_DIR — images for visual inspection */

#define TU_MAX_FILES 128
#define TU_DIR_MODE 0755
#define TU_IMAGE_WIDTH_MAX 320UL
#define TU_IMAGE_HEIGHT_MAX 240UL
#define TU_FRAME_BUF_LEN (TU_IMAGE_WIDTH_MAX * TU_IMAGE_HEIGHT_MAX)

void tu_ensure_parent_dir(const char *path);

typedef struct {
    uint8_t buf[TU_IMAGE_WIDTH_MAX * TU_IMAGE_HEIGHT_MAX * 3];
    size_t width;
    size_t height;
    size_t channels;
} tu_image_t;
