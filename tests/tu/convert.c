#include "tu/convert.h"

/* Exact PIL BT.601 coefficients with rounding: (19595*R + 38470*G + 7471*B + 32768)
 * >> 16 */
#define GRAY_R 19595U
#define GRAY_G 38470U
#define GRAY_B 7471U
#define GRAY_ROUND 32768U
#define GRAY_SHIFT 16U
#define RGB_CHANNELS 3U
#define BINARIZE_UP 255U

void tu_to_frames(tu_image_t *imgs, gauge_frame_t *out, size_t count) {
    for (size_t idx = 0; idx < count; idx++) {
        tu_image_t *img = &imgs[idx];
        size_t pixels = img->width * img->height;

        for (size_t i = 0; i < pixels; i++) {
            uint8_t red = img->buf[(i * RGB_CHANNELS) + 0];
            uint8_t green = img->buf[(i * RGB_CHANNELS) + 1];
            uint8_t blue = img->buf[(i * RGB_CHANNELS) + 2];
            img->buf[i] = (uint8_t) ((GRAY_R * red + GRAY_G * green + GRAY_B * blue +
                                      GRAY_ROUND) >>
                                     GRAY_SHIFT);
        }

        img->channels = 1;
        out[idx].buf = img->buf;
        out[idx].buf_len = pixels;
        out[idx].width = img->width;
        out[idx].height = img->height;
    }
}

void tu_from_frame(const gauge_frame_t *frame, tu_image_t *out) {
    size_t pixels = frame->width * frame->height;

    for (size_t i = pixels; i > 0; i--) {
        uint8_t gray = frame->buf[i - 1];
        out->buf[((i - 1) * RGB_CHANNELS) + 0] = gray;
        out->buf[((i - 1) * RGB_CHANNELS) + 1] = gray;
        out->buf[((i - 1) * RGB_CHANNELS) + 2] = gray;
    }

    out->width = frame->width;
    out->height = frame->height;
    out->channels = RGB_CHANNELS;
}

void tu_unbinarize(gauge_frame_t *frame) {
    for (size_t i = 0; i < frame->buf_len; i++) {
        if (frame->buf[i] != 0) {
            frame->buf[i] = BINARIZE_UP;
        }
    }
}
