#pragma once

#include "gauge.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>

/* Load JPEG from path into tu_image_t (RGB).
 * Returns false on error. */
bool tu_image_load(const char *path, tu_image_t *out);

/* Save tu_image_t as JPEG to path.
 * Returns false on error. */
bool tu_image_save(const char *path, const tu_image_t *img);

/* --- Drawing --- */

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} tu_color_t;

#define TU_RED ((tu_color_t) {255, 0, 0})
#define TU_GREEN ((tu_color_t) {0, 255, 0})
#define TU_BLUE ((tu_color_t) {0, 0, 255})

void tu_draw_point(tu_image_t *img, float x_pos, float y_pos, tu_color_t color);

void tu_draw_line(tu_image_t *img, const gauge_line_t *line, tu_color_t color);

void tu_draw_vector(tu_image_t *img, float pivot_x, float pivot_y, float angle_rad,
                    float length, tu_color_t color);

void tu_draw_calibration(tu_image_t *img, const gauge_calibration_data_t *ca_data);
