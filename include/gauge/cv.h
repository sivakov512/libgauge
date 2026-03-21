#pragma once

#include "gauge.h"
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Computes the background frame as the per-pixel maximum across all frames.
 *
 * @param frames      Array of grayscale frames of identical size.
 * @param frames_len  Number of frames.
 * @param bg_out      Output frame; must be pre-allocated with the same dimensions.
 */
void gauge_cv_calculate_background(const gauge_frame_t *frames, size_t frames_len,
                                   gauge_frame_t *bg_out);

/**
 * Subtracts background from frame in-place.
 *
 * Each pixel becomes |pixel - bg_pixel|.
 *
 * @param frame  Frame to modify in-place.
 * @param bg     Background frame of identical size.
 */
void gauge_cv_subtract_background(gauge_frame_t *frame, const gauge_frame_t *bg);

/**
 * Binarizes frame in-place.
 *
 * Pixels above threshold become 1, all others become 0.
 *
 * @param frame      Frame to modify in-place.
 * @param threshold  Intensity threshold.
 */
void gauge_cv_binarize(gauge_frame_t *frame, uint8_t threshold);

/**
 * Extracts the largest connected blob in-place, zeroing all other blobs.
 *
 * @param frame  Binarized frame to modify in-place.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_BLOB_NOT_FOUND if no blob pixels are present.
 * @return GAUGE_ERR_TOO_MANY_BLOBS if the number of blobs exceeds UINT8_MAX (255).
 */
gauge_err_t gauge_cv_extract_largest_blob(gauge_frame_t *frame);

/**
 * Fits a line through the largest blob using principal component analysis.
 *
 * Expects a binarized frame where blob pixels have value 1.
 *
 * @param frame     Binarized frame containing a single blob.
 * @param line_out  Output line fitted to the blob.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_BLOB_NOT_FOUND if no blob pixels are present.
 */
gauge_err_t gauge_cv_blob_to_line(const gauge_frame_t *frame,
                                  gauge_line_t *line_out);

/**
 * Computes the intersection point of two infinite lines.
 *
 * @param line1             First line.
 * @param line2             Second line.
 * @param intersection_out  Output intersection point.
 * @return GAUGE_OK on success.
 * @return GAUGE_ERR_AXES_NOT_INTERSECTING if lines are parallel or nearly parallel.
 */
gauge_err_t gauge_cv_intersect_lines(const gauge_line_t *line1,
                                     const gauge_line_t *line2,
                                     gauge_pointf_t *intersection_out);

/**
 * Returns the maximum distance in pixels from pivot to any blob pixel in frame.
 *
 * Expects a binarized frame where blob pixels have value 1.
 *
 * @param frame  Binarized frame containing a blob.
 * @param pivot  Reference point to measure distance from.
 * @return       Maximum distance in pixels (0 if no blob pixels are present).
 */
size_t gauge_cv_arrow_length(const gauge_frame_t *frame,
                             const gauge_pointf_t *pivot);

#ifdef __cplusplus
}
#endif
