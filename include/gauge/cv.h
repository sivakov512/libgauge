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

gauge_err_t gauge_extract_largest_blob(gauge_frame_t *frame);

gauge_err_t gauge_cv_blob_to_line(const gauge_frame_t *frame,
                                  gauge_line_t *line_out);

gauge_err_t gauge_cv_intersect_lines(const gauge_line_t *line1,
                                     const gauge_line_t *line2,
                                     gauge_point_t *intersection_out);

#ifdef __cplusplus
}
#endif
