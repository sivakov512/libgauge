#pragma once

#include "gauge.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void gauge_cv__calculate_background(gauge_frame_t *frames, size_t frames_len,
                                    gauge_frame_t *bg_out);

#ifdef __cplusplus
}
#endif
