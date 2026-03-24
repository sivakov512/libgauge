#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"
#include <string.h>

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN] = {0};
static size_t g_scratch[GAUGE_EXTRACT_BLOB_SCRATCH_SIZE(TU_FRAME_BUF_LEN)];
static gauge_frame_t g_frame;

void setUp() {
    g_frame = (gauge_frame_t) {
        .buf = g_frame_buf,
        .buf_len = TU_FRAME_BUF_LEN,
        .width = TU_IMAGE_WIDTH_MAX,
        .height = TU_IMAGE_HEIGHT_MAX,
    };
}

void tearDown() {}

static void test_extract_largest_blob(const char *frame_path, const char *name) {
    FIXTURES_LOAD_FRAME(frame_path, &g_frame);

    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_extract_largest_blob(&g_frame, g_scratch,
                                                              TU_FRAME_BUF_LEN));

    SNAPSHOT_ASSERT_FRAME(name, &g_frame);

    tu_unbinarize(&g_frame);
    EXAMPLES_SAVE_FRAME(name, &g_frame);
}

#define CASES(X)                                                                    \
    X(test_extract_largest_blob__set_1_00767591132,                                 \
      "set/1/00767591132_binarized.json")                                           \
    X(test_extract_largest_blob__set_1_12756622577,                                 \
      "set/1/12756622577_binarized.json")

#define DEF_TEST(name, frame)                                                       \
    static void name() {                                                            \
        test_extract_largest_blob(frame, #name);                                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

#define SYNTHETIC_FRAME_SIZE ((size_t) 16)
#define SYNTHETIC_FRAME_BUF_LEN (SYNTHETIC_FRAME_SIZE * SYNTHETIC_FRAME_SIZE)

static void test_works_well_if_frame_is_a_full_of_blob() {
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .buf_len = SYNTHETIC_FRAME_BUF_LEN,
                               .width = SYNTHETIC_FRAME_SIZE,
                               .height = SYNTHETIC_FRAME_SIZE};
    memset(g_frame_buf, 1, SYNTHETIC_FRAME_BUF_LEN);

    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_extract_largest_blob(
                                    &g_frame, g_scratch, SYNTHETIC_FRAME_BUF_LEN));

    TEST_ASSERT_EACH_EQUAL_UINT8(1, g_frame.buf, g_frame.buf_len);
}

static void test_errored_if_scratch_too_small() {
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .buf_len = SYNTHETIC_FRAME_BUF_LEN,
                               .width = SYNTHETIC_FRAME_SIZE,
                               .height = SYNTHETIC_FRAME_SIZE};
    memset(g_frame_buf, 1, SYNTHETIC_FRAME_BUF_LEN);

    TEST_ASSERT_EQUAL(GAUGE_ERR_SCRATCH_BUF_TOO_SMALL,
                      gauge_cv_extract_largest_blob(&g_frame, g_scratch,
                                                    SYNTHETIC_FRAME_BUF_LEN - 1));
}

static void test_errored_if_frame_has_no_blobs() {
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .buf_len = SYNTHETIC_FRAME_BUF_LEN,
                               .width = SYNTHETIC_FRAME_SIZE,
                               .height = SYNTHETIC_FRAME_SIZE};
    memset(g_frame_buf, 0, SYNTHETIC_FRAME_BUF_LEN);

    TEST_ASSERT_EQUAL(
        GAUGE_ERR_BLOB_NOT_FOUND,
        gauge_cv_extract_largest_blob(&g_frame, g_scratch, SYNTHETIC_FRAME_BUF_LEN));
}

// Blob spacing of 3 ensures no 8-connected neighbors between blobs.
// 18x18 grid of blobs in a 54x54 frame = 324 blobs > UINT8_MAX.
#define TOO_MANY_FRAME_SIZE ((size_t) 54)
#define TOO_MANY_FRAME_BUF_LEN (TOO_MANY_FRAME_SIZE * TOO_MANY_FRAME_SIZE)
#define TOO_MANY_SPACING ((size_t) 3)

static void test_errored_if_frame_has_too_many_blobs() {
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .buf_len = TOO_MANY_FRAME_BUF_LEN,
                               .width = TOO_MANY_FRAME_SIZE,
                               .height = TOO_MANY_FRAME_SIZE};
    memset(g_frame_buf, 0, TOO_MANY_FRAME_BUF_LEN);
    for (size_t row = 0; row < TOO_MANY_FRAME_SIZE; row += TOO_MANY_SPACING) {
        for (size_t col = 0; col < TOO_MANY_FRAME_SIZE; col += TOO_MANY_SPACING) {
            g_frame_buf[(row * TOO_MANY_FRAME_SIZE) + col] = 1;
        }
    }

    TEST_ASSERT_EQUAL(
        GAUGE_ERR_TOO_MANY_BLOBS,
        gauge_cv_extract_largest_blob(&g_frame, g_scratch, TOO_MANY_FRAME_BUF_LEN));
}

int main() {
    UNITY_BEGIN();

#define RUN(name, frame) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_works_well_if_frame_is_a_full_of_blob);
    RUN_TEST(test_errored_if_scratch_too_small);
    RUN_TEST(test_errored_if_frame_has_no_blobs);
    RUN_TEST(test_errored_if_frame_has_too_many_blobs);

    return UNITY_END();
}
