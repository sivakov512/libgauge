#pragma once

#include "gauge.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void gauge_cv_calculate_background(const gauge_frame_t *frames, size_t frames_len,
                                   gauge_frame_t *bg_out);

void gauge_cv_subtract_background(gauge_frame_t *frame, const gauge_frame_t *bg);

void gauge_cv_binarize(gauge_frame_t *frame, uint8_t threshold);

#ifdef __cplusplus
}
#endif
