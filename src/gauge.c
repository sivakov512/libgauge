#include "gauge.h"
#include <stddef.h>
#include <stdlib.h>

#define BINARIZE_UP 1
#define BINARIZE_DOWN 0

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
