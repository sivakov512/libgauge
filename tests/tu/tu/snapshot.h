#pragma once

#include "gauge.h"

/* Snapshot behaviour:
 *   - UPDATE_SNAPSHOTS=1 → write snapshot and pass.
 *   - File absent       → fail with message.
 *   - File present      → compare, fail via Unity if not equal.
 *
 * Use the CAPS macros — TEST_SNAPSHOTS_DIR is injected automatically:
 *   TU_SNAPSHOT_ASSERT_FRAME(name, frame)
 *   TU_SNAPSHOT_ASSERT_LINE(name, line)
 *   TU_SNAPSHOT_ASSERT_CALIBRATION(name, ca_data)
 *   TU_SNAPSHOT_ASSERT_FLOAT(name, value, epsilon) */

void tu_snapshot_assert_frame(const char *snapshots_dir, const char *name,
                              const gauge_frame_t *frame);

void tu_snapshot_assert_line(const char *snapshots_dir, const char *name,
                             const gauge_line_t *line);

void tu_snapshot_assert_calibration(const char *snapshots_dir, const char *name,
                                    const gauge_calibration_data_t *ca_data);

void tu_snapshot_assert_float(const char *snapshots_dir, const char *name,
                              float value, float epsilon);

#define TU_SNAPSHOT_ASSERT_FRAME(name, frame)                                       \
    tu_snapshot_assert_frame(TEST_SNAPSHOTS_DIR, (name), (frame))

#define TU_SNAPSHOT_ASSERT_LINE(name, line)                                         \
    tu_snapshot_assert_line(TEST_SNAPSHOTS_DIR, (name), (line))

#define TU_SNAPSHOT_ASSERT_CALIBRATION(name, ca_data)                               \
    tu_snapshot_assert_calibration(TEST_SNAPSHOTS_DIR, (name), (ca_data))

#define TU_SNAPSHOT_ASSERT_FLOAT(name, value, epsilon)                              \
    tu_snapshot_assert_float(TEST_SNAPSHOTS_DIR, (name), (value), (epsilon))
