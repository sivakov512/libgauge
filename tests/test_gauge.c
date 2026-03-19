#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "tu/utils.h"
#include "unity.h"

#include <stdint.h>
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

static void test_calculate_background(const char *source_dir, const char *name) {
    static tu_image_t imgs[TU_MAX_FILES];
    size_t count = TU_FIXTURES_LOAD_IMAGES(source_dir, imgs, TU_MAX_FILES);
    TEST_ASSERT_GREATER_THAN(0, count);

    static gauge_frame_t frames[TU_MAX_FILES];
    tu_to_frames(imgs, frames, count);

    uint8_t bg_buf[TU_FRAME_BUF_LEN] = {0};
    gauge_frame_t background = {.buf = bg_buf,
                                .buf_len = frames[0].buf_len,
                                .width = frames[0].width,
                                .height = frames[0].height};
    gauge_cv_calculate_background(frames, count, &background);

    TU_SNAPSHOT_ASSERT_FRAME(name, &background);

    TU_EXAMPLES_SAVE_FRAME(name, &background);
}

#define CALCULATE_BACKGROUND_CASES(X) X(test_calculate_background__set_1, "set/1")

#define DEF_TEST(name, dir)                                                         \
    static void name(void) {                                                        \
        test_calculate_background(dir, #name);                                      \
    }
CALCULATE_BACKGROUND_CASES(DEF_TEST)
#undef DEF_TEST

static void test_subtract_background(const char *image_path, const char *bg_path,
                                     const char *name) {
    tu_image_t img;
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_IMAGE(image_path, &img));

    static gauge_frame_t frame;
    tu_to_frames(&img, &frame, 1);

    static uint8_t bg_buf[TU_FRAME_BUF_LEN];
    static gauge_frame_t background;
    TEST_ASSERT_TRUE(
        TU_FIXTURES_LOAD_FRAME(bg_path, bg_buf, TU_FRAME_BUF_LEN, &background));

    gauge_cv_subtract_background(&frame, &background);

    TU_SNAPSHOT_ASSERT_FRAME(name, &frame);
    TU_EXAMPLES_SAVE_FRAME(name, &frame);
}

#define BINARIZE_THRESHOLD 20

static void test_binarize(const char *frame_path, const char *name) {
    static uint8_t frame_buf[TU_FRAME_BUF_LEN];
    static gauge_frame_t frame;
    TEST_ASSERT_TRUE(
        TU_FIXTURES_LOAD_FRAME(frame_path, frame_buf, TU_FRAME_BUF_LEN, &frame));

    gauge_cv_binarize(&frame, BINARIZE_THRESHOLD);

    TU_SNAPSHOT_ASSERT_FRAME(name, &frame);

    tu_unbinarize(&frame);
    TU_EXAMPLES_SAVE_FRAME(name, &frame);
}

#define BINARIZE_CASES(X)                                                           \
    X(test_binarize__set_1_00767591132, "set/1/00767591132_subtracted.json")        \
    X(test_binarize__set_1_12756622577, "set/1/12756622577_subtracted.json")

#define DEF_TEST(name, frame)                                                       \
    static void name(void) {                                                        \
        test_binarize(frame, #name);                                                \
    }
BINARIZE_CASES(DEF_TEST)
#undef DEF_TEST

static void test_extract_largest_blob(const char *frame_path, const char *name) {
    static uint8_t frame_buf[TU_FRAME_BUF_LEN];
    static gauge_frame_t frame;
    TEST_ASSERT_TRUE(
        TU_FIXTURES_LOAD_FRAME(frame_path, frame_buf, TU_FRAME_BUF_LEN, &frame));

    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_extract_largest_blob(&frame));

    TU_SNAPSHOT_ASSERT_FRAME(name, &frame);

    tu_unbinarize(&frame);
    TU_EXAMPLES_SAVE_FRAME(name, &frame);
}

#define EXTRACT_LARGEST_BLOB_CASES(X)                                               \
    X(test_extract_largest_blob__set_1_00767591132,                                 \
      "set/1/00767591132_binarized.json")                                           \
    X(test_extract_largest_blob__set_1_12756622577,                                 \
      "set/1/12756622577_binarized.json")

#define DEF_TEST(name, frame)                                                       \
    static void name(void) {                                                        \
        test_extract_largest_blob(frame, #name);                                    \
    }
EXTRACT_LARGEST_BLOB_CASES(DEF_TEST)
#undef DEF_TEST

#define SYNTHETIC_FRAME_SIZE ((size_t) 16)
#define SYNTHETIC_FRAME_BUF_LEN (SYNTHETIC_FRAME_SIZE * SYNTHETIC_FRAME_SIZE)

// Blob spacing of 3 ensures no 8-connected neighbors between blobs.
// 18x18 grid of blobs in a 54x54 frame = 324 blobs > UINT8_MAX.
#define TOO_MANY_BLOBS_FRAME_SIZE ((size_t) 54)
#define TOO_MANY_BLOBS_FRAME_BUF_LEN                                                \
    (TOO_MANY_BLOBS_FRAME_SIZE * TOO_MANY_BLOBS_FRAME_SIZE)
#define TOO_MANY_BLOBS_SPACING ((size_t) 3)

static void test_extract_largest_blob__single_blob(void) {
    static uint8_t buf[SYNTHETIC_FRAME_BUF_LEN];
    memset(buf, 1, sizeof(buf));
    gauge_frame_t frame = {.buf = buf,
                           .buf_len = SYNTHETIC_FRAME_BUF_LEN,
                           .width = SYNTHETIC_FRAME_SIZE,
                           .height = SYNTHETIC_FRAME_SIZE};
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_extract_largest_blob(&frame));
    for (size_t ii = 0; ii < frame.buf_len; ++ii) {
        TEST_ASSERT_EQUAL_UINT8(1, frame.buf[ii]);
    }
}

static void test_extract_largest_blob__blob_not_found(void) {
    static uint8_t buf[SYNTHETIC_FRAME_BUF_LEN];
    memset(buf, 0, sizeof(buf));
    gauge_frame_t frame = {.buf = buf,
                           .buf_len = SYNTHETIC_FRAME_BUF_LEN,
                           .width = SYNTHETIC_FRAME_SIZE,
                           .height = SYNTHETIC_FRAME_SIZE};
    TEST_ASSERT_EQUAL(GAUGE_ERR_BLOB_NOT_FOUND, gauge_extract_largest_blob(&frame));
}

static void test_extract_largest_blob__too_many_blobs(void) {
    static uint8_t buf[TOO_MANY_BLOBS_FRAME_BUF_LEN];
    memset(buf, 0, sizeof(buf));
    for (size_t row = 0; row < TOO_MANY_BLOBS_FRAME_SIZE;
         row += TOO_MANY_BLOBS_SPACING) {
        for (size_t col = 0; col < TOO_MANY_BLOBS_FRAME_SIZE;
             col += TOO_MANY_BLOBS_SPACING) {
            buf[(row * TOO_MANY_BLOBS_FRAME_SIZE) + col] = 1;
        }
    }
    gauge_frame_t frame = {.buf = buf,
                           .buf_len = TOO_MANY_BLOBS_FRAME_BUF_LEN,
                           .width = TOO_MANY_BLOBS_FRAME_SIZE,
                           .height = TOO_MANY_BLOBS_FRAME_SIZE};
    TEST_ASSERT_EQUAL(GAUGE_ERR_TOO_MANY_BLOBS, gauge_extract_largest_blob(&frame));
}

#define SUBTRACT_BACKGROUND_CASES(X)                                                \
    X(test_subtract_background__set_1_00767591132, "set/1/00767591132.jpg",         \
      "set/1/background.json")                                                      \
    X(test_subtract_background__set_1_12756622577, "set/1/12756622577.jpg",         \
      "set/1/background.json")

#define DEF_TEST(name, image, bg)                                                   \
    static void name(void) {                                                        \
        test_subtract_background(image, bg, #name);                                 \
    }
SUBTRACT_BACKGROUND_CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, dir) RUN_TEST(name);
    CALCULATE_BACKGROUND_CASES(RUN)
#undef RUN

#define RUN(name, image, bg) RUN_TEST(name);
    SUBTRACT_BACKGROUND_CASES(RUN)
#undef RUN

#define RUN(name, frame) RUN_TEST(name);
    BINARIZE_CASES(RUN)
#undef RUN

#define RUN(name, frame) RUN_TEST(name);
    EXTRACT_LARGEST_BLOB_CASES(RUN)
#undef RUN

    RUN_TEST(test_extract_largest_blob__single_blob);
    RUN_TEST(test_extract_largest_blob__blob_not_found);
    RUN_TEST(test_extract_largest_blob__too_many_blobs);

    return UNITY_END();
}
