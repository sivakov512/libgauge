#pragma once

#include "gauge.h"

/** Pi constant (float precision). */
#define GAUGE_PI 3.141593F

/** Two pi constant (float precision). */
#define GAUGE_TWO_PI 6.283185F

/**
 * Normalizes an angle to [-pi, pi] and adjusts for spin direction.
 *
 * After normalization, applies direction adjustment to ensure the angle has the
 * correct sign for iterative radial scanning across the +/-pi boundary:
 * - GAUGE_SPIN_CW (>0): maps negative angles to positive range (adds 2*pi).
 * - GAUGE_SPIN_CCW (<0): maps positive angles to negative range (subtracts 2*pi).
 * - 0: no direction adjustment; result stays in [-pi, pi].
 *
 * @param angle_rad  Input angle in radians.
 * @param direction  Spin direction (GAUGE_SPIN_CW, GAUGE_SPIN_CCW, or 0).
 * @return           Normalized angle in radians.
 */
float gauge_utils_normalize_angle(float angle_rad, gauge_spin_t direction);
