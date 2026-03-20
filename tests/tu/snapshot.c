#include "tu/snapshot.h"
#include "tu/json.h"
#include "tu/utils.h"
#include "unity.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const float ANGLE_EPSILON = 1e-5F;

static bool should_update(void) {
    const char *val = getenv("UPDATE_SNAPSHOTS");
    return val != NULL && strcmp(val, "1") == 0;
}

static bool file_exists(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        return false;
    }
    fclose(file);
    return true;
}

static void build_path(char *buf, size_t buf_len, const char *snapshots_dir,
                       const char *name) {
    snprintf(buf, buf_len, "%s/%s.json", snapshots_dir, name);
}

static uint8_t g_frame_read_buf[TU_FRAME_BUF_LEN];

void tu_snapshot_assert_frame(const char *snapshots_dir, const char *name,
                              const gauge_frame_t *frame) {
    char path[PATH_MAX];
    build_path(path, sizeof(path), snapshots_dir, name);

    if (should_update()) {
        tu_ensure_parent_dir(path);
        TEST_ASSERT_TRUE(tu_json_write_frame(path, frame));
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(
        file_exists(path),
        "Snapshot not found. Run with UPDATE_SNAPSHOTS=1 to create it.");

    gauge_frame_t stored = {.buf = g_frame_read_buf,
                            .buf_len = sizeof(g_frame_read_buf)};
    TEST_ASSERT_TRUE(tu_json_read_frame(path, &stored));
    TEST_ASSERT_EQUAL_size_t(frame->width, stored.width);
    TEST_ASSERT_EQUAL_size_t(frame->height, stored.height);
    TEST_ASSERT_EQUAL_size_t(frame->buf_len, stored.buf_len);
    TEST_ASSERT_EQUAL_MEMORY(frame->buf, stored.buf, frame->buf_len);
}

void tu_snapshot_assert_line(const char *snapshots_dir, const char *name,
                             const gauge_line_t *line) {
    char path[PATH_MAX];
    build_path(path, sizeof(path), snapshots_dir, name);

    if (should_update()) {
        tu_ensure_parent_dir(path);
        TEST_ASSERT_TRUE(tu_json_write_line(path, line));
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(
        file_exists(path),
        "Snapshot not found. Run with UPDATE_SNAPSHOTS=1 to create it.");

    gauge_line_t stored;
    TEST_ASSERT_TRUE(tu_json_read_line(path, &stored));
    TEST_ASSERT_FLOAT_WITHIN(ANGLE_EPSILON, stored.origin.x, line->origin.x);
    TEST_ASSERT_FLOAT_WITHIN(ANGLE_EPSILON, stored.origin.y, line->origin.y);
    TEST_ASSERT_FLOAT_WITHIN(ANGLE_EPSILON, stored.direction.x, line->direction.x);
    TEST_ASSERT_FLOAT_WITHIN(ANGLE_EPSILON, stored.direction.y, line->direction.y);
}

void tu_snapshot_assert_calibration(const char *snapshots_dir, const char *name,
                                    const gauge_calibration_data_t *ca_data) {
    char path[PATH_MAX];
    build_path(path, sizeof(path), snapshots_dir, name);

    if (should_update()) {
        tu_ensure_parent_dir(path);
        TEST_ASSERT_TRUE(tu_json_write_calibration(path, ca_data));
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(
        file_exists(path),
        "Snapshot not found. Run with UPDATE_SNAPSHOTS=1 to create it.");

    gauge_calibration_data_t stored;
    TEST_ASSERT_TRUE(tu_json_read_calibration(path, &stored));
    TEST_ASSERT_EQUAL_size_t(ca_data->pivot.x, stored.pivot.x);
    TEST_ASSERT_EQUAL_size_t(ca_data->pivot.y, stored.pivot.y);
    TEST_ASSERT_EQUAL_INT8(ca_data->spin, stored.spin);
    TEST_ASSERT_EQUAL_size_t(ca_data->arrow_len, stored.arrow_len);
    TEST_ASSERT_FLOAT_WITHIN(ANGLE_EPSILON, ca_data->angle_start_rad,
                             stored.angle_start_rad);
    TEST_ASSERT_FLOAT_WITHIN(ANGLE_EPSILON, ca_data->angle_end_rad,
                             stored.angle_end_rad);
}

void tu_snapshot_assert_float(const char *snapshots_dir, const char *name,
                              float value, float epsilon) {
    char path[PATH_MAX];
    build_path(path, sizeof(path), snapshots_dir, name);

    if (should_update()) {
        tu_ensure_parent_dir(path);
        TEST_ASSERT_TRUE(tu_json_write_float(path, value));
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(
        file_exists(path),
        "Snapshot not found. Run with UPDATE_SNAPSHOTS=1 to create it.");

    float stored;
    TEST_ASSERT_TRUE(tu_json_read_float(path, &stored));
    TEST_ASSERT_FLOAT_WITHIN(epsilon, value, stored);
}

void tu_snapshot_assert_pointf(const char *snapshots_dir, const char *name,
                               const gauge_pointf_t *point, float epsilon) {
    char path[PATH_MAX];
    build_path(path, sizeof(path), snapshots_dir, name);

    if (should_update()) {
        tu_ensure_parent_dir(path);
        TEST_ASSERT_TRUE(tu_json_write_pointf(path, point));
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(
        file_exists(path),
        "Snapshot not found. Run with UPDATE_SNAPSHOTS=1 to create it.");

    gauge_pointf_t stored;
    TEST_ASSERT_TRUE(tu_json_read_pointf(path, &stored));
    TEST_ASSERT_FLOAT_WITHIN(epsilon, point->x, stored.x);
    TEST_ASSERT_FLOAT_WITHIN(epsilon, point->y, stored.y);
}

void tu_snapshot_assert_size_t(const char *snapshots_dir, const char *name,
                               size_t value) {
    char path[PATH_MAX];
    build_path(path, sizeof(path), snapshots_dir, name);

    if (should_update()) {
        tu_ensure_parent_dir(path);
        TEST_ASSERT_TRUE(tu_json_write_size_t(path, value));
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(
        file_exists(path),
        "Snapshot not found. Run with UPDATE_SNAPSHOTS=1 to create it.");

    size_t stored;
    TEST_ASSERT_TRUE(tu_json_read_size_t(path, &stored));
    TEST_ASSERT_EQUAL_size_t(value, stored);
}
