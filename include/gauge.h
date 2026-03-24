#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t gauge_spin_t;

#define GAUGE_SPIN_UNKNOWN ((gauge_spin_t) 0)
#define GAUGE_SPIN_CW ((gauge_spin_t) 1)
#define GAUGE_SPIN_CCW ((gauge_spin_t) - 1)

/** Default pixel intensity difference threshold for binarization. */
#define GAUGE_BINARIZATION_THRESHOLD 20

/** Radial scan angular step in radians (~0.5 degrees). */
#define GAUGE_RADIAL_SCAN_STEP 0.008726646F

/**
 * Single grayscale video frame.
 *
 * Pixels are stored row-major: pixel at (x, y) is at index y*width + x.
 * buf must point to a buffer of at least buf_len bytes.
 */
typedef struct {
    uint8_t *buf;
    size_t buf_len;

    size_t width;
    size_t height;
} gauge_frame_t;

/** 2D point with floating-point coordinates. */
typedef struct {
    float x;
    float y;
} gauge_pointf_t;

/** Infinite line defined by a point and a unit direction vector. */
typedef struct {
    gauge_pointf_t origin;
    gauge_pointf_t direction;
} gauge_line_t;

/** Gauge geometry data used for arrow angle measurement. */
typedef struct {
    gauge_pointf_t pivot; /**< Arrow rotation center in frame coordinates. */
    float
        angle_start_rad; /**< Arrow angle at the minimum scale position (radians). */
    float angle_end_rad; /**< Arrow angle at the maximum scale position (radians). */
    gauge_spin_t spin;   /**< Rotation direction: GAUGE_SPIN_CW, GAUGE_SPIN_CCW, or
                            GAUGE_SPIN_UNKNOWN if not yet determined. */
    size_t arrow_len;    /**< Arrow length in pixels from pivot to tip. */
} gauge_calibration_data_t;

typedef enum {
    GAUGE_OK = 0,
    GAUGE_ERR_BLOB_NOT_FOUND,
    GAUGE_ERR_TOO_MANY_BLOBS,
    GAUGE_ERR_AXES_NOT_INTERSECTING,
    GAUGE_ERR_SPIN_UNDETERMINED,
    GAUGE_ERR_FRAME_SIZE_MISMATCH,
} gauge_err_t;

/**
 * Returns the buffer index of pixel at (pos_x, pos_y).
 *
 * @param frame  Frame to index into.
 * @param pos_x  Pixel column (0-based).
 * @param pos_y  Pixel row (0-based).
 * @return       Index into frame->buf.
 */
static inline size_t gauge_frame_pixel_index(const gauge_frame_t *frame,
                                             size_t pos_x, size_t pos_y) {
    return (pos_y * frame->width) + pos_x;
}

/**
 * Calibrates gauge geometry from a sequence of frames.
 *
 * Expects frames captured while the arrow moves through its full range.
 * Computes the rotation pivot by subtracting background from the first and last
 * frames, fitting a line through the arrow blob in each, and intersecting the two
 * lines. Spin direction is determined from intermediate frames.
 *
 * @param frames                  Array of grayscale frames of identical size.
 * @param frames_len              Number of frames; must be at least 3.
 * @param binarization_threshold  Pixel intensity threshold for binarization after
 *                                background subtraction.
 * @param ca_data_out             Output calibration data.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_FRAME_SIZE_MISMATCH if frames differ in size.
 * @return GAUGE_ERR_BLOB_NOT_FOUND if no arrow blob is detected.
 * @return GAUGE_ERR_TOO_MANY_BLOBS if blob count exceeds UINT8_MAX (255).
 * @return GAUGE_ERR_AXES_NOT_INTERSECTING if the two arrow lines are parallel.
 * @return GAUGE_ERR_SPIN_UNDETERMINED if spin direction cannot be determined
 *         (fewer than 3 frames).
 */
gauge_err_t gauge_calibrate_by_axis_intersection(
    gauge_frame_t *frames, size_t frames_len, uint8_t binarization_threshold,
    gauge_calibration_data_t *ca_data_out);

/**
 * Measures the current arrow angle by radial scanning.
 *
 * Scans from ca_data->angle_start_rad to ca_data->angle_end_rad in steps of
 * radial_scan_step, accumulating pixel scores along each radial line from the
 * pivot. Writes the best-match angle to angle_out.
 *
 * @param frame             Grayscale frame to scan.
 * @param ca_data           Calibration data describing gauge geometry.
 * @param radial_scan_step  Angular step between scanned directions (radians).
 * @param angle_out         Best-match angle in radians, normalized to [-pi, pi].
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_SPIN_UNDETERMINED if ca_data->spin is GAUGE_SPIN_UNKNOWN.
 */
gauge_err_t gauge_scan_radial(const gauge_frame_t *frame,
                              const gauge_calibration_data_t *ca_data,
                              float radial_scan_step, float *angle_out);

#ifdef __cplusplus
}
#endif
