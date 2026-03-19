#pragma once

#include "gauge.h"
#include <stdbool.h>
#include <stdint.h>

/* Read gauge_frame_t from JSON file. buf must be provided by caller.
 * Returns false on error. */
bool tu_json_read_frame(const char *path, uint8_t *buf, size_t buf_len,
                        gauge_frame_t *out);

/* Write gauge_frame_t to JSON file.
 * Returns false on error. */
bool tu_json_write_frame(const char *path, const gauge_frame_t *frame);

/* Read gauge_line_t from JSON file.
 * Returns false on error. */
bool tu_json_read_line(const char *path, gauge_line_t *out);

/* Write gauge_line_t to JSON file.
 * Returns false on error. */
bool tu_json_write_line(const char *path, const gauge_line_t *line);

/* Read gauge_calibration_data_t from JSON file.
 * Returns false on error. */
bool tu_json_read_calibration(const char *path, gauge_calibration_data_t *out);

/* Write gauge_calibration_data_t to JSON file.
 * Returns false on error. */
bool tu_json_write_calibration(const char *path,
                               const gauge_calibration_data_t *ca_data);

/* Read a single float scalar from JSON file.
 * Returns false on error. */
bool tu_json_read_float(const char *path, float *out);

/* Write a single float scalar to JSON file.
 * Returns false on error. */
bool tu_json_write_float(const char *path, float value);
