#include "gauge.h"
#include "gauge/utils.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// --- Utils ---

float gauge_utils_normalize_angle(float angle_rad, gauge_spin_t direction) {
    while (angle_rad > GAUGE_PI) {
        angle_rad -= GAUGE_TWO_PI;
    }
    while (angle_rad < -GAUGE_PI) {
        angle_rad += GAUGE_TWO_PI;
    }

    if (direction > 0 && angle_rad < 0) {
        angle_rad += GAUGE_TWO_PI;
    } else if (direction < 0 && angle_rad > 0) {
        angle_rad -= GAUGE_TWO_PI;
    }

    return angle_rad;
}

// --- CV ---

#define BINARIZE_UP 1
#define BINARIZE_DOWN 0

#define NEIGHBORHOOD_SIZE 8
static const int NEIGHBORHOOD[NEIGHBORHOOD_SIZE][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1},
};

#define KINDA_ZERO 1e-6F

#define DIRECTION_SEARCH_STEP_RAD 0.175F

static inline size_t size_t_max(size_t first, size_t second) {
    return first > second ? first : second;
}

void gauge_cv_subtract_background(gauge_frame_t *frame, const gauge_frame_t *bg) {
    for (size_t i = 0; i < frame->buf_len; ++i) {
        uint8_t pixel = frame->buf[i];
        uint8_t bg_pixel = bg->buf[i];
        frame->buf[i] = pixel > bg_pixel ? pixel - bg_pixel : bg_pixel - pixel;
    }
}

void gauge_cv_binarize(gauge_frame_t *frame, uint8_t threshold) {
    for (size_t i = 0; i < frame->buf_len; ++i) {
        if (frame->buf[i] > threshold) {
            frame->buf[i] = BINARIZE_UP;
        } else {
            frame->buf[i] = BINARIZE_DOWN;
        }
    }
}

static size_t flood_fill(gauge_frame_t *frame, size_t index, uint8_t label,
                         size_t *stack) {
    size_t *stack_top = stack;

    frame->buf[index] = label;
    *stack_top++ = index;
    size_t size = 1;

    while (stack_top != stack) {
        size_t idx = *--stack_top;
        size_t cur_x = idx % frame->width;
        size_t cur_y = idx / frame->width;

        for (uint8_t i = 0; i < NEIGHBORHOOD_SIZE; ++i) {
            int nx = (int) cur_x + NEIGHBORHOOD[i][0];
            int ny = (int) cur_y + NEIGHBORHOOD[i][1];

            if (nx >= 0 && (size_t) nx < frame->width && ny >= 0 &&
                (size_t) ny < frame->height) {
                size_t ni = gauge_frame_pixel_index(frame, (size_t) nx, (size_t) ny);
                if (frame->buf[ni] == BINARIZE_UP) {
                    frame->buf[ni] = label;
                    *stack_top++ = ni;
                    ++size;
                }
            }
        }
    }

    return size;
}

gauge_err_t gauge_cv_extract_largest_blob(gauge_frame_t *frame, size_t *flood_stack,
                                          size_t flood_stack_len) {
    if (flood_stack_len < frame->buf_len) {
        return GAUGE_ERR_SCRATCH_BUF_TOO_SMALL;
    }

    uint8_t max_label = 0;
    size_t max_label_size = 0;

    uint8_t label = BINARIZE_UP + 1;
    for (size_t index = 0; index < frame->buf_len; ++index) {
        if (label == UINT8_MAX) {
            return GAUGE_ERR_TOO_MANY_BLOBS;
        }

        if (frame->buf[index] == BINARIZE_UP) {
            size_t label_size = flood_fill(frame, index, label, flood_stack);
            if (label_size > max_label_size) {
                max_label = label;
                max_label_size = label_size;
            }
            ++label;
        }
    }

    if (max_label == 0) {
        return GAUGE_ERR_BLOB_NOT_FOUND;
    }

    for (size_t i = 0; i < frame->buf_len; ++i) {
        frame->buf[i] = frame->buf[i] == max_label ? BINARIZE_UP : BINARIZE_DOWN;
    }
    return GAUGE_OK;
}

static gauge_err_t center_of_mass(const gauge_frame_t *frame,
                                  gauge_pointf_t *point_out) {
    float sum_x = 0;
    float sum_y = 0;
    size_t count = 0;

    for (size_t pos_y = 0; pos_y < frame->height; ++pos_y) {
        for (size_t pos_x = 0; pos_x < frame->width; ++pos_x) {
            size_t index = gauge_frame_pixel_index(frame, pos_x, pos_y);

            if (frame->buf[index] == BINARIZE_UP) {
                sum_x += (float) pos_x;
                sum_y += (float) pos_y;
                ++count;
            }
        }
    }

    if (count == 0) {
        return GAUGE_ERR_BLOB_NOT_FOUND;
    }

    point_out->x = sum_x / (float) count;
    point_out->y = sum_y / (float) count;
    return GAUGE_OK;
}

static void principal_axis(const gauge_frame_t *frame, const gauge_pointf_t *mpoint,
                           gauge_pointf_t *vector_out) {
    float cxx = 0;
    float cxy = 0;
    float cyy = 0;
    size_t count = 0;

    // Covariance matrix
    for (size_t pos_y = 0; pos_y < frame->height; ++pos_y) {
        for (size_t pos_x = 0; pos_x < frame->width; ++pos_x) {
            if (frame->buf[gauge_frame_pixel_index(frame, pos_x, pos_y)] ==
                BINARIZE_UP) {
                float delta_x = (float) pos_x - mpoint->x;
                float delta_y = (float) pos_y - mpoint->y;
                cxx += delta_x * delta_x;
                cxy += delta_x * delta_y;
                cyy += delta_y * delta_y;
                ++count;
            }
        }
    }

    cxx /= (float) count;
    cxy /= (float) count;
    cyy /= (float) count;

    // lambda1
    float trace = cxx + cyy;
    float det = (cxx * cyy) - (cxy * cxy);
    float D = // NOLINT(readability-identifier-naming,readability-identifier-length)
        (trace * trace) - (4.0F * det);      // NOLINT(readability-magic-numbers)
    float sqrt_D = sqrtf(fmaxf(0, D));       // NOLINT(readability-identifier-naming)
    float lambda1 = (trace + sqrt_D) / 2.0F; // NOLINT(readability-magic-numbers)

    // lambda1 vector
    if (fabsf(cxy) > KINDA_ZERO) {
        vector_out->x = cxy;
        vector_out->y = lambda1 - cxx;
    } else {
        if (cxx > cyy) {
            vector_out->x = 1.0F;
            vector_out->y = 0.0F;
        } else {
            vector_out->x = 0.0F;
            vector_out->y = 1.0F;
        }
    }

    // Normalize vector length
    float len =
        sqrtf((vector_out->x * vector_out->x) + (vector_out->y * vector_out->y));
    vector_out->x /= len;
    vector_out->y /= len;
}

gauge_err_t gauge_cv_blob_to_line(const gauge_frame_t *frame,
                                  gauge_line_t *line_out) {
    gauge_err_t err = center_of_mass(frame, &line_out->origin);
    if (err != GAUGE_OK) {
        return err;
    }

    principal_axis(frame, &line_out->origin, &line_out->direction);
    return GAUGE_OK;
}

gauge_err_t gauge_cv_intersect_lines(const gauge_line_t *line1,
                                     const gauge_line_t *line2,
                                     gauge_pointf_t *intersection_out) {
    float delta_x = line2->origin.x - line1->origin.x;
    float delta_y = line2->origin.y - line1->origin.y;

    float cross = (line1->direction.x * line2->direction.y) -
                  (line1->direction.y * line2->direction.x);

    if (fabsf(cross) < KINDA_ZERO) {
        return GAUGE_ERR_AXES_NOT_INTERSECTING;
    }

    float param =
        ((delta_x * line2->direction.y) - (delta_y * line2->direction.x)) / cross;
    intersection_out->x = line1->origin.x + (param * line1->direction.x);
    intersection_out->y = line1->origin.y + (param * line1->direction.y);
    return GAUGE_OK;
}

size_t gauge_cv_arrow_length(const gauge_frame_t *frame,
                             const gauge_pointf_t *pivot) {
    size_t max_dist = 0;
    for (size_t pos_y = 0; pos_y < frame->height; ++pos_y) {
        for (size_t pos_x = 0; pos_x < frame->width; ++pos_x) {
            size_t index = gauge_frame_pixel_index(frame, pos_x, pos_y);

            if (frame->buf[index] == BINARIZE_UP) {
                float delta_x = (float) pos_x - pivot->x;
                float delta_y = (float) pos_y - pivot->y;
                size_t dist = (size_t) roundf(
                    sqrtf((delta_x * delta_x) + (delta_y * delta_y)));
                if (dist > max_dist) {
                    max_dist = dist;
                }
            }
        }
    }
    return max_dist;
}

// --- Calibration API ---

static gauge_err_t frame_angle(gauge_frame_t *frame, const gauge_frame_t *bg,
                               uint8_t threshold, const gauge_pointf_t *pivot,
                               size_t *scratch, size_t scratch_len,
                               float *angle_out);

gauge_err_t gauge_update_background(const gauge_frame_t *frame, gauge_frame_t *bg) {
    if (frame->width != bg->width || frame->height != bg->height ||
        frame->buf_len != bg->buf_len) {
        return GAUGE_ERR_FRAME_SIZE_MISMATCH;
    }

    for (size_t i = 0; i < frame->buf_len; ++i) {
        if (frame->buf[i] > bg->buf[i]) {
            bg->buf[i] = frame->buf[i];
        }
    }
    return GAUGE_OK;
}

gauge_err_t gauge_calibrate_spin(gauge_frame_t *frame, const gauge_frame_t *bg,
                                 uint8_t binarization_threshold, size_t *scratch,
                                 size_t scratch_len,
                                 gauge_calibration_data_t *ca_data) {
    if (frame->width != bg->width || frame->height != bg->height ||
        frame->buf_len != bg->buf_len) {
        return GAUGE_ERR_FRAME_SIZE_MISMATCH;
    }

    float angle;
    gauge_err_t err = frame_angle(frame, bg, binarization_threshold, &ca_data->pivot,
                                  scratch, scratch_len, &angle);
    if (err != GAUGE_OK) {
        return err;
    }

    float diff = angle - ca_data->angle_start_rad;
    if (fabsf(diff) <= DIRECTION_SEARCH_STEP_RAD) {
        return GAUGE_ERR_SPIN_UNDETERMINED;
    }

    ca_data->spin = diff > 0 ? GAUGE_SPIN_CW : GAUGE_SPIN_CCW;
    return GAUGE_OK;
}

static gauge_err_t frame_line(gauge_frame_t *frame, const gauge_frame_t *bg,
                              uint8_t threshold, size_t *scratch, size_t scratch_len,
                              gauge_line_t *line_out) {
    gauge_cv_subtract_background(frame, bg);
    gauge_cv_binarize(frame, threshold);
    gauge_err_t err = gauge_cv_extract_largest_blob(frame, scratch, scratch_len);
    if (err != GAUGE_OK) {
        return err;
    }

    return gauge_cv_blob_to_line(frame, line_out);
}

static gauge_err_t frame_angle(gauge_frame_t *frame, const gauge_frame_t *bg,
                               uint8_t threshold, const gauge_pointf_t *pivot,
                               size_t *scratch, size_t scratch_len,
                               float *angle_out) {
    gauge_cv_subtract_background(frame, bg);
    gauge_cv_binarize(frame, threshold);
    gauge_err_t err = gauge_cv_extract_largest_blob(frame, scratch, scratch_len);
    if (err != GAUGE_OK) {
        return err;
    }

    gauge_pointf_t point;
    err = center_of_mass(frame, &point);
    if (err != GAUGE_OK) {
        return err;
    }

    *angle_out = atan2f(point.y - pivot->y, point.x - pivot->x);
    return GAUGE_OK;
}

gauge_err_t gauge_calibrate_by_axis_intersection(
    gauge_frame_t *first, gauge_frame_t *last, const gauge_frame_t *bg,
    uint8_t binarization_threshold, size_t *scratch, size_t scratch_len,
    gauge_calibration_data_t *ca_data_out) {
    if (first->width != last->width || first->height != last->height ||
        first->buf_len != last->buf_len || first->width != bg->width ||
        first->height != bg->height || first->buf_len != bg->buf_len) {
        return GAUGE_ERR_FRAME_SIZE_MISMATCH;
    }

    gauge_line_t start_line = {0};
    gauge_err_t err = frame_line(first, bg, binarization_threshold, scratch,
                                 scratch_len, &start_line);
    if (err != GAUGE_OK) {
        return err;
    }

    gauge_line_t end_line = {0};
    err = frame_line(last, bg, binarization_threshold, scratch, scratch_len,
                     &end_line);
    if (err != GAUGE_OK) {
        return err;
    }

    gauge_pointf_t pivot;
    err = gauge_cv_intersect_lines(&start_line, &end_line, &pivot);
    if (err != GAUGE_OK) {
        return err;
    }

    float start_angle =
        atan2f(start_line.origin.y - pivot.y, start_line.origin.x - pivot.x);
    float end_angle =
        atan2f(end_line.origin.y - pivot.y, end_line.origin.x - pivot.x);
    size_t length = size_t_max(gauge_cv_arrow_length(first, &pivot),
                               gauge_cv_arrow_length(last, &pivot));

    *ca_data_out = (gauge_calibration_data_t) {
        .pivot = pivot,
        .angle_start_rad = start_angle,
        .angle_end_rad = end_angle,
        .spin = GAUGE_SPIN_UNKNOWN,
        .arrow_len = length,
    };
    return GAUGE_OK;
}

// --- Scan radial ---

gauge_err_t gauge_scan_radial(const gauge_frame_t *frame,
                              const gauge_calibration_data_t *ca_data,
                              float radial_scan_step, float *angle_out) {
    if (ca_data->spin == GAUGE_SPIN_UNKNOWN) {
        return GAUGE_ERR_SPIN_UNDETERMINED;
    }

    float scan_diff = gauge_utils_normalize_angle(
        ca_data->angle_end_rad - ca_data->angle_start_rad, ca_data->spin);
    float angle_step = radial_scan_step * (float) ca_data->spin;

    float best_angle = ca_data->angle_start_rad;
    int best_score = -1;

    float angle = ca_data->angle_start_rad;
    size_t steps = (size_t) fabsf(scan_diff / angle_step) + 1;
    for (size_t i = 0; i < steps; ++i) {
        int score = 0;
        float cos_a = cosf(angle);
        float sin_a = sinf(angle);

        for (size_t arrl = 0; arrl < ca_data->arrow_len; ++arrl) {
            int x = (int) roundf(ca_data->pivot.x + ((float) arrl * cos_a));
            int y = (int) roundf(ca_data->pivot.y + ((float) arrl * sin_a));

            if (x >= 0 && (size_t) x < frame->width && y >= 0 &&
                (size_t) y < frame->height) {
                size_t index =
                    gauge_frame_pixel_index(frame, (size_t) x, (size_t) y);
                uint8_t value = frame->buf[index];
                score += UINT8_MAX - value;
            }
        }

        if (score > best_score) {
            best_score = score;
            best_angle = angle;
        }
        angle += angle_step;
    }

    *angle_out = gauge_utils_normalize_angle(best_angle, 0);
    return GAUGE_OK;
}
