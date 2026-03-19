#include "tu/image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "tu/utils.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define JPEG_QUALITY 95
#define IMAGE_CHANNELS 3
#define POINT_RADIUS 3
#define LINE_THICKNESS 1
#define RGB_CHANNELS 3U

bool tu_image_load(const char *path, tu_image_t *out) {
    int width;
    int height;
    int channels;

    unsigned char *data =
        stbi_load(path, &width, &height, &channels, IMAGE_CHANNELS);
    if (!data) {
        return false;
    }

    if ((size_t) width > TU_IMAGE_WIDTH_MAX ||
        (size_t) height > TU_IMAGE_HEIGHT_MAX) {
        stbi_image_free(data);
        return false;
    }

    memcpy(out->buf, data, (size_t) width * (size_t) height * IMAGE_CHANNELS);
    stbi_image_free(data);

    out->width = (size_t) width;
    out->height = (size_t) height;
    out->channels = IMAGE_CHANNELS;
    return true;
}

bool tu_image_save(const char *path, const tu_image_t *img) {
    tu_ensure_parent_dir(path);
    return stbi_write_jpg(path, (int) img->width, (int) img->height, IMAGE_CHANNELS,
                          img->buf, JPEG_QUALITY) != 0;
}

static void put_pixel(tu_image_t *img, int col, int row, tu_color_t color) {
    if (col < 0 || row < 0 || (size_t) col >= img->width ||
        (size_t) row >= img->height) {
        return;
    }
    size_t idx = (((size_t) row * img->width) + (size_t) col) * RGB_CHANNELS;
    img->buf[idx + 0] = color.r;
    img->buf[idx + 1] = color.g;
    img->buf[idx + 2] = color.b;
}

void tu_draw_point(tu_image_t *img, float x_pos, float y_pos, tu_color_t color) {
    for (int dy = -POINT_RADIUS; dy <= POINT_RADIUS; dy++) {
        for (int dx = -POINT_RADIUS; dx <= POINT_RADIUS; dx++) {
            put_pixel(img, (int) x_pos + dx, (int) y_pos + dy, color);
        }
    }
}

void tu_draw_line(tu_image_t *img, const gauge_line_t *line, tu_color_t color) {
    float orig_x = (float) line->origin.x;
    float orig_y = (float) line->origin.y;
    float dir_x = (float) line->direction.x;
    float dir_y = (float) line->direction.y;
    float len = sqrtf((dir_x * dir_x) + (dir_y * dir_y));
    if (len == 0.0F) {
        return;
    }
    dir_x /= len;
    dir_y /= len;

    int max_t = (int) (img->width + img->height);

    for (int offset = -LINE_THICKNESS; offset <= LINE_THICKNESS; offset++) {
        for (int step = -max_t; step <= max_t; step++) {
            int col = (int) (orig_x + ((float) step * dir_x) +
                             ((float) offset * (-dir_y)));
            int row =
                (int) (orig_y + ((float) step * dir_y) + ((float) offset * dir_x));
            put_pixel(img, col, row, color);
        }
    }
}

void tu_draw_vector(tu_image_t *img, float pivot_x, float pivot_y, float angle_rad,
                    float length, tu_color_t color) {
    float cos_a = cosf(angle_rad);
    float sin_a = sinf(angle_rad);

    for (int offset = -LINE_THICKNESS; offset <= LINE_THICKNESS; offset++) {
        for (int step = 0; step <= (int) length; step++) {
            int col = (int) (pivot_x + ((float) step * cos_a) +
                             ((float) offset * (-sin_a)));
            int row =
                (int) (pivot_y + ((float) step * sin_a) + ((float) offset * cos_a));
            put_pixel(img, col, row, color);
        }
    }
}

void tu_draw_calibration(tu_image_t *img, const gauge_calibration_data_t *ca_data) {
    float pivot_x = (float) ca_data->pivot.x;
    float pivot_y = (float) ca_data->pivot.y;
    float arrow_len = (float) ca_data->arrow_len;

    tu_draw_vector(img, pivot_x, pivot_y, ca_data->angle_start_rad, arrow_len,
                   TU_BLUE);
    tu_draw_vector(img, pivot_x, pivot_y, ca_data->angle_end_rad, arrow_len, TU_RED);
    tu_draw_point(img, pivot_x, pivot_y, TU_GREEN);
}
