#include "gauge.h"

void gauge_cv__calculate_background(gauge_frame_t *frames, size_t frames_len,
                                    gauge_frame_t *bg_out) {
    for (size_t frame_i = 0; frame_i < frames_len; ++frame_i) {
        gauge_frame_t *frame = &frames[frame_i];
        for (size_t pixel_i = 0; pixel_i < frame->buf_len; ++pixel_i) {
            if (frame->buf[pixel_i] > bg_out->buf[pixel_i]) {
                bg_out->buf[pixel_i] = frame->buf[pixel_i];
            }
        }
    }
}
