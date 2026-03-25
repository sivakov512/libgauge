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
 * Minimum angular displacement in radians required to determine spin direction
 * in gauge_calibrate_spin (~10 degrees).
 */
#define GAUGE_CALIBRATE_SPIN_MIN_ANGLE_RAD 0.175F

/**
 * Single grayscale video frame.
 *
 * Pixels are stored row-major: pixel at (x, y) is at index y*width + x.
 * buf must point to a buffer of at least width * height bytes.
 */
typedef struct {
    uint8_t *buf;
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
    GAUGE_ERR_SCRATCH_BUF_TOO_SMALL,
} gauge_err_t;

/**
 * Returns the total number of pixels (and bytes) in a frame buffer.
 *
 * @param frame  Frame to query.
 * @return       frame->width * frame->height.
 */
static inline size_t gauge_frame_buf_len(const gauge_frame_t *frame) {
    return frame->width * frame->height;
}

/**
 * Minimum number of size_t elements required for the scratch buffer passed to
 * gauge_cv_extract_largest_blob, gauge_calibrate_spin, and
 * gauge_calibrate_by_axis_intersection.
 *
 * Use this to declare a scratch buffer at compile time:
 *   size_t scratch[GAUGE_SCRATCH_SIZE(WIDTH, HEIGHT)];
 *
 * @param width   Frame width in pixels.
 * @param height  Frame height in pixels.
 */
#define GAUGE_SCRATCH_SIZE(width, height) ((width) * (height))

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
 * Updates the background frame with a new frame using per-pixel maximization.
 *
 * Call once per frame with bg->buf zeroed on the first call. After processing
 * all frames, bg->buf holds the per-pixel maximum across the entire sequence.
 *
 * @param frame  Grayscale frame to incorporate into the background.
 * @param bg     Background frame to update in-place; must have the same
 *               dimensions as frame.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_FRAME_SIZE_MISMATCH if frame and bg differ in dimensions.
 */
gauge_err_t gauge_update_background(const gauge_frame_t *frame, gauge_frame_t *bg);

/**
 * Calibrates gauge geometry from two extreme-position frames.
 *
 * Subtracts bg from first and last, binarizes, fits a line through each arrow
 * blob, and intersects the two lines to find the rotation pivot. Arrow angles
 * and length are computed from the blob positions.
 *
 * ca_data_out->spin is set to GAUGE_SPIN_UNKNOWN; call gauge_calibrate_spin
 * afterward to determine direction.
 *
 * first and last are modified in-place (background subtraction, binarization,
 * blob extraction).
 *
 * @param first                   Frame at the minimum arrow position; modified
 *                                in-place.
 * @param last                    Frame at the maximum arrow position; modified
 *                                in-place.
 * @param bg                      Background frame; must match first and last in
 *                                dimensions.
 * @param binarization_threshold  Pixel intensity threshold for binarization.
 * @param scratch      Scratch buffer; must hold at least
 *                     GAUGE_SCRATCH_SIZE(first->width, first->height) elements.
 * @param scratch_len  Number of size_t elements in scratch.
 * @param ca_data_out  Output calibration data; spin is GAUGE_SPIN_UNKNOWN.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_FRAME_SIZE_MISMATCH if first, last, or bg differ in size.
 * @return GAUGE_ERR_BLOB_NOT_FOUND if no arrow blob is detected.
 * @return GAUGE_ERR_TOO_MANY_BLOBS if blob count exceeds 253.
 * @return GAUGE_ERR_AXES_NOT_INTERSECTING if the two arrow lines are parallel.
 * @return GAUGE_ERR_SCRATCH_BUF_TOO_SMALL if scratch_len <
 *         GAUGE_SCRATCH_SIZE(first->width, first->height).
 */
gauge_err_t gauge_calibrate_by_axis_intersection(
    gauge_frame_t *first, gauge_frame_t *last, const gauge_frame_t *bg,
    uint8_t binarization_threshold, size_t *scratch, size_t scratch_len,
    gauge_calibration_data_t *ca_data_out);

/**
 * Determines the rotation direction of the arrow from a single frame.
 *
 * Subtracts the background, binarizes, extracts the arrow blob, and computes
 * the arrow angle relative to ca_data->pivot. Compares it to
 * ca_data->angle_start_rad: if the angular difference exceeds the detection
 * threshold, writes the spin direction into ca_data->spin and returns GAUGE_OK.
 * If the arrow has not moved enough to determine direction, returns
 * GAUGE_ERR_SPIN_UNDETERMINED without modifying ca_data.
 *
 * frame is modified in-place (background subtraction, binarization, blob
 * extraction).
 *
 * @param frame                   Grayscale frame to analyse; modified in-place.
 * @param bg                      Background frame; must have the same dimensions
 *                                as frame.
 * @param binarization_threshold  Pixel intensity threshold for binarization.
 * @param scratch      Scratch buffer; must hold at least
 *                     GAUGE_SCRATCH_SIZE(frame->width, frame->height) elements.
 * @param scratch_len  Number of size_t elements in scratch.
 * @param ca_data      Calibration data with pivot and angle_start_rad already
 *                     filled; spin is written on success.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_SPIN_UNDETERMINED if the angular difference is too small.
 * @return GAUGE_ERR_FRAME_SIZE_MISMATCH if frame and bg differ in dimensions.
 * @return GAUGE_ERR_BLOB_NOT_FOUND if no arrow blob is detected.
 * @return GAUGE_ERR_TOO_MANY_BLOBS if blob count exceeds 253.
 * @return GAUGE_ERR_SCRATCH_BUF_TOO_SMALL if scratch_len <
 *         GAUGE_SCRATCH_SIZE(frame->width, frame->height).
 */
gauge_err_t gauge_calibrate_spin(gauge_frame_t *frame, const gauge_frame_t *bg,
                                 uint8_t binarization_threshold, size_t *scratch,
                                 size_t scratch_len,
                                 gauge_calibration_data_t *ca_data);

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
