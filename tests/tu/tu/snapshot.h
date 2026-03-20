#pragma once

#include "gauge.h"

/* Snapshot behaviour:
 *   - UPDATE_SNAPSHOTS=1 → write snapshot and pass.
 *   - File absent       → fail with message.
 *   - File present      → compare, fail via Unity if not equal.
 *
 * Use the CAPS macros — TEST_SNAPSHOTS_DIR is injected automatically:
 *   SNAPSHOT_ASSERT_FRAME(name, frame)
 *   SNAPSHOT_ASSERT_LINE(name, line)
 *   SNAPSHOT_ASSERT_CALIBRATION(name, ca_data)
 *   SNAPSHOT_ASSERT_FLOAT(name, value, epsilon)
 *   SNAPSHOT_ASSERT_POINTF(name, point, epsilon)
 *   SNAPSHOT_ASSERT_SIZE_T(name, value) */

void tu_snapshot_assert_frame(const char *snapshots_dir, const char *name,
                              const gauge_frame_t *frame);

void tu_snapshot_assert_line(const char *snapshots_dir, const char *name,
                             const gauge_line_t *line);

void tu_snapshot_assert_calibration(const char *snapshots_dir, const char *name,
                                    const gauge_calibration_data_t *ca_data);

void tu_snapshot_assert_float(const char *snapshots_dir, const char *name,
                              float value, float epsilon);

void tu_snapshot_assert_pointf(const char *snapshots_dir, const char *name,
                               const gauge_pointf_t *point, float epsilon);

void tu_snapshot_assert_size_t(const char *snapshots_dir, const char *name,
                               size_t value);

#define SNAPSHOT_ASSERT_FRAME(name, frame)                                          \
    tu_snapshot_assert_frame(TEST_SNAPSHOTS_DIR, (name), (frame))

#define SNAPSHOT_ASSERT_LINE(name, line)                                            \
    tu_snapshot_assert_line(TEST_SNAPSHOTS_DIR, (name), (line))

#define SNAPSHOT_ASSERT_CALIBRATION(name, ca_data)                                  \
    tu_snapshot_assert_calibration(TEST_SNAPSHOTS_DIR, (name), (ca_data))

#define SNAPSHOT_ASSERT_FLOAT(name, value, epsilon)                                 \
    tu_snapshot_assert_float(TEST_SNAPSHOTS_DIR, (name), (value), (epsilon))

#define SNAPSHOT_ASSERT_POINTF(name, point, epsilon)                                \
    tu_snapshot_assert_pointf(TEST_SNAPSHOTS_DIR, (name), (point), (epsilon))

#define SNAPSHOT_ASSERT_SIZE_T(name, value)                                         \
    tu_snapshot_assert_size_t(TEST_SNAPSHOTS_DIR, (name), (value))
