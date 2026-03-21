#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t gauge_spin_t;

#define GAUGE_SPIN_CW ((gauge_spin_t) 1)
#define GAUGE_SPIN_CCW ((gauge_spin_t) - 1)

#define GAUGE_BINARIZATION_THRESHOLD 20
#define GAUGE_RADIAL_SCAN_STEP 0.008726646F

typedef struct {
    uint8_t *buf;
    size_t buf_len;

    size_t width;
    size_t height;
} gauge_frame_t;

typedef struct {
    float x;
    float y;
} gauge_pointf_t;

typedef struct {
    gauge_pointf_t origin;
    gauge_pointf_t direction;
} gauge_line_t;

typedef struct {
    gauge_pointf_t pivot;
    float angle_start_rad;
    float angle_end_rad;
    gauge_spin_t spin;
    size_t arrow_len;
} gauge_calibration_data_t;

typedef enum {
    GAUGE_OK = 0,
    GAUGE_ERR_BLOB_NOT_FOUND,
    GAUGE_ERR_TOO_MANY_BLOBS,
    GAUGE_ERR_AXES_NOT_INTERSECTING,
    GAUGE_ERR_SPIN_UNDETERMINED,
    GAUGE_ERR_FRAME_SIZE_MISMATCH,
} gauge_err_t;

static inline size_t gauge_frame_pixel_index(const gauge_frame_t *frame,
                                             size_t pos_x, size_t pos_y) {
    return (pos_y * frame->width) + pos_x;
}

gauge_err_t gauge_calibrate_by_axis_intersection(
    gauge_frame_t *frames, size_t frames_len, uint8_t binarization_threshold,
    gauge_calibration_data_t *ca_data_out);
float gauge_scan_radial(const gauge_frame_t *frame,
                        const gauge_calibration_data_t *ca_data,
                        float radial_scan_step);

#ifdef __cplusplus
}
#endif
