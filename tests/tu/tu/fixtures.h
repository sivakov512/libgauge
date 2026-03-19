#pragma once

#include "gauge.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Load single JPEG from fixtures_dir/rel_path. Returns false on error. */
bool tu_fixtures_load_image(const char *fixtures_dir, const char *rel_path,
                            tu_image_t *out);

/* Load all JPEGs from fixtures_dir/rel_dir, sorted by filename.
 * Returns number of images loaded. */
size_t tu_fixtures_load_images(const char *fixtures_dir, const char *rel_dir,
                               tu_image_t *out, size_t max_count);

/* Load gauge_frame_t from JSON at fixtures_dir/rel_path.
 * Returns false on error. */
bool tu_fixtures_load_frame(const char *fixtures_dir, const char *rel_path,
                            uint8_t *buf, size_t buf_len, gauge_frame_t *out);

/* Load gauge_line_t from JSON at fixtures_dir/rel_path.
 * Returns false on error. */
bool tu_fixtures_load_line(const char *fixtures_dir, const char *rel_path,
                           gauge_line_t *out);

/* Load gauge_calibration_data_t from JSON at fixtures_dir/rel_path.
 * Returns false on error. */
bool tu_fixtures_load_calibration(const char *fixtures_dir, const char *rel_path,
                                  gauge_calibration_data_t *out);

#define TU_FIXTURES_LOAD_IMAGE(rel_path, out)                                       \
    tu_fixtures_load_image(TEST_FIXTURES_DIR, (rel_path), (out))

#define TU_FIXTURES_LOAD_IMAGES(rel_dir, out, max_count)                            \
    tu_fixtures_load_images(TEST_FIXTURES_DIR, (rel_dir), (out), (max_count))

#define TU_FIXTURES_LOAD_FRAME(rel_path, buf, buf_len, out)                         \
    tu_fixtures_load_frame(TEST_FIXTURES_DIR, (rel_path), (buf), (buf_len), (out))

#define TU_FIXTURES_LOAD_LINE(rel_path, out)                                        \
    tu_fixtures_load_line(TEST_FIXTURES_DIR, (rel_path), (out))

#define TU_FIXTURES_LOAD_CALIBRATION(rel_path, out)                                 \
    tu_fixtures_load_calibration(TEST_FIXTURES_DIR, (rel_path), (out))
