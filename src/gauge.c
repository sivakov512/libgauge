#include "gauge.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BINARIZE_UP 1
#define BINARIZE_DOWN 0

#define NEIGHBORHOOD_SIZE 8
static const size_t NEIGHBORHOOD[NEIGHBORHOOD_SIZE][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1},
};

void gauge_cv_calculate_background(const gauge_frame_t *frames, size_t frames_len,
                                   gauge_frame_t *bg_out) {
    for (size_t frame_i = 0; frame_i < frames_len; ++frame_i) {
        const gauge_frame_t *frame = &frames[frame_i];
        for (size_t pixel_i = 0; pixel_i < frame->buf_len; ++pixel_i) {
            if (frame->buf[pixel_i] > bg_out->buf[pixel_i]) {
                bg_out->buf[pixel_i] = frame->buf[pixel_i];
            }
        }
    }
}

void gauge_cv_subtract_background(gauge_frame_t *frame, const gauge_frame_t *bg) {
    for (size_t i = 0; i < frame->buf_len; ++i) {
        frame->buf[i] = abs(frame->buf[i] - bg->buf[i]);
    }
}

void gauge_cv_binarize(gauge_frame_t *frame, uint8_t threshold) {
    for (size_t i = 0; i < frame->buf_len; ++i) {
        if (frame->buf[i] > threshold) {
            frame->buf[i] = BINARIZE_UP;
        } else {
            frame->buf[i] = BINARIZE_DOWN;
        }
    }
}

static size_t flood_fill(gauge_frame_t *frame, size_t index, uint8_t label,
                         size_t *stack) {
    size_t *stack_top = stack;

    *stack_top++ = index;
    size_t size = 0;

    while (stack_top != stack) {
        size_t index = *--stack_top;

        if (frame->buf[index] != BINARIZE_UP) {
            continue;
        }

        frame->buf[index] = label;
        ++size;

        size_t cur_x = index % frame->width;
        size_t cur_y = index / frame->width;

        for (uint8_t i = 0; i < NEIGHBORHOOD_SIZE; ++i) {
            size_t nx = cur_x + NEIGHBORHOOD[i][0]; // NOLINT
            size_t ny = cur_y + NEIGHBORHOOD[i][1]; // NOLINT

            if ((0 <= nx && nx < frame->width) && (0 <= ny && ny < frame->height)) {
                size_t ni = gauge_frame_pixel_index(frame, nx, ny); // NOLINT
                if (frame->buf[ni] == BINARIZE_UP) {
                    *stack_top++ = ni;
                }
            }
        }
    }

    return size;
}

gauge_err_t gauge_extract_largest_blob(gauge_frame_t *frame) {
    uint8_t max_label = 0;
    size_t max_label_size = 0;

    gauge_err_t err = GAUGE_OK;
    size_t *stack = malloc(sizeof(size_t) * frame->buf_len);

    uint8_t label = BINARIZE_UP + 1;
    for (size_t index = 0; index < frame->buf_len; ++index) {
        if (label == UINT8_MAX) {
            err = GAUGE_ERR_TOO_MANY_BLOBS;
            goto ret;
        }

        if (frame->buf[index] == BINARIZE_UP) {
            size_t label_size = flood_fill(frame, index, label, stack);
            if (label_size > max_label_size) {
                max_label = label;
                max_label_size = label_size;
            }
            ++label;
        }
    }

    if (max_label == 0) {
        err = GAUGE_ERR_BLOB_NOT_FOUND;
        goto ret;
    }

    for (size_t i = 0; i < frame->buf_len; ++i) {
        if (frame->buf[i] == max_label) {
            frame->buf[i] = BINARIZE_UP;
        } else {
            frame->buf[i] = BINARIZE_DOWN;
        }
    }

ret:
    free(stack);
    return err;
}
