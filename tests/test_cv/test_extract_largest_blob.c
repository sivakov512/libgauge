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

/* clang-format off */
#define CASES(X) \
    X(test_extract_largest_blob__set_1_00767591132, "set/1/00767591132_binarized.json") \
    X(test_extract_largest_blob__set_1_12756622577, "set/1/12756622577_binarized.json")
/* clang-format on */

#define DEF_TEST(name, frame)                                                       \
    static void name(void) {                                                        \
        test_extract_largest_blob(frame, #name);                                    \
    }
CASES(DEF_TEST)
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

int main(void) {
    UNITY_BEGIN();

#define RUN(name, frame) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_extract_largest_blob__single_blob);
    RUN_TEST(test_extract_largest_blob__blob_not_found);
    RUN_TEST(test_extract_largest_blob__too_many_blobs);

    return UNITY_END();
}
