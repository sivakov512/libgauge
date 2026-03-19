#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t gauge_spin_t;

#define GAUGE_SPIN_CW ((gauge_spin_t) 1)
#define GAUGE_SPIN_CCW ((gauge_spin_t) - 1)

typedef struct {
    uint8_t *buf;
    size_t buf_len;

    size_t width;
    size_t height;
} gauge_frame_t;

typedef struct {
    size_t x;
    size_t y;
} gauge_point_t;

typedef struct {
    gauge_point_t origin;
    gauge_point_t direction;
} gauge_line_t;

typedef struct {
    gauge_point_t pivot;
    float angle_start_rad;
    float angle_end_rad;
    gauge_spin_t spin;
    size_t arrow_len;
} gauge_calibration_data_t;

typedef enum {
    GAUGE_ERR_OK = 0,
    GAUGE_ERR_BLOB_NOT_FOUND,
    GAUGE_ERR_AXES_NOT_INTERSECTING,
    GAUGE_ERR_SPIN_UNDETERMINED,
} gauge_err_t;

gauge_err_t gauge_calibrate(gauge_frame_t *frames, size_t frames_len,
                            gauge_calibration_data_t *ca_data_out);
gauge_err_t gauge_measure(gauge_frame_t *frame, gauge_calibration_data_t *ca_data,
                          float *angle_rad_out);

#ifdef __cplusplus
}
#endif
