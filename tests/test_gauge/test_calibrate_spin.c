#include "gauge.h"
#include "tu/convert.h"
#include "tu/fixtures.h"
#include "unity.h"
#include <string.h>

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN];
static uint8_t g_bg_buf[TU_FRAME_BUF_LEN];
static tu_image_t g_img;
static gauge_frame_t g_frame;
static gauge_frame_t g_bg;
static gauge_calibration_data_t g_ca_data;

void setUp() {
    memset(g_frame_buf, 0, sizeof(g_frame_buf));
    memset(g_bg_buf, 0, sizeof(g_bg_buf));

    g_frame = (gauge_frame_t) {
        .buf = g_frame_buf,
        .buf_len = TU_FRAME_BUF_LEN,
        .width = TU_IMAGE_WIDTH_MAX,
        .height = TU_IMAGE_HEIGHT_MAX,
    };
    g_bg = (gauge_frame_t) {
        .buf = g_bg_buf,
        .buf_len = TU_FRAME_BUF_LEN,
        .width = TU_IMAGE_WIDTH_MAX,
        .height = TU_IMAGE_HEIGHT_MAX,
    };
}

void tearDown() {}

static void test_errored_if_arrow_displacement_too_small(void) {
    FIXTURES_LOAD_CALIBRATION("set/1/ca_data_no_spin.json", &g_ca_data);
    FIXTURES_LOAD_FRAME("set/1/background.json", &g_bg);
    FIXTURES_LOAD_IMAGE("set/1/01356622581.jpg", &g_img);
    tu_to_frames(&g_img, &g_frame, 1);

    gauge_err_t err = gauge_calibrate_spin(&g_frame, &g_bg,
                                           GAUGE_BINARIZATION_THRESHOLD, &g_ca_data);

    TEST_ASSERT_EQUAL(GAUGE_ERR_SPIN_UNDETERMINED, err);
    TEST_ASSERT_EQUAL(GAUGE_SPIN_UNKNOWN, g_ca_data.spin);
}

static void test_determines_spin_when_arrow_displaced_enough(void) {
    FIXTURES_LOAD_CALIBRATION("set/1/ca_data_no_spin.json", &g_ca_data);
    FIXTURES_LOAD_FRAME("set/1/background.json", &g_bg);
    FIXTURES_LOAD_IMAGE("set/1/02556622581.jpg", &g_img);
    tu_to_frames(&g_img, &g_frame, 1);

    gauge_err_t err = gauge_calibrate_spin(&g_frame, &g_bg,
                                           GAUGE_BINARIZATION_THRESHOLD, &g_ca_data);

    TEST_ASSERT_EQUAL(GAUGE_OK, err);
    TEST_ASSERT_EQUAL(GAUGE_SPIN_CCW, g_ca_data.spin);
}

// Blob spacing of 3 ensures no 8-connected neighbors between blobs.
// 18x18 grid of blobs in a 54x54 frame = 324 blobs > UINT8_MAX.
#define TOO_MANY_FRAME_SIZE ((size_t) 54)
#define TOO_MANY_FRAME_BUF_LEN (TOO_MANY_FRAME_SIZE * TOO_MANY_FRAME_SIZE)
#define TOO_MANY_SPACING ((size_t) 3)

static void test_errored_if_too_many_blobs(void) {
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .buf_len = TOO_MANY_FRAME_BUF_LEN,
                               .width = TOO_MANY_FRAME_SIZE,
                               .height = TOO_MANY_FRAME_SIZE};
    g_bg = (gauge_frame_t) {.buf = g_bg_buf,
                            .buf_len = TOO_MANY_FRAME_BUF_LEN,
                            .width = TOO_MANY_FRAME_SIZE,
                            .height = TOO_MANY_FRAME_SIZE};
    for (size_t row = 0; row < TOO_MANY_FRAME_SIZE; row += TOO_MANY_SPACING) {
        for (size_t col = 0; col < TOO_MANY_FRAME_SIZE; col += TOO_MANY_SPACING) {
            g_frame_buf[(row * TOO_MANY_FRAME_SIZE) + col] = UINT8_MAX;
        }
    }

    gauge_err_t err = gauge_calibrate_spin(&g_frame, &g_bg,
                                           GAUGE_BINARIZATION_THRESHOLD, &g_ca_data);

    TEST_ASSERT_EQUAL(GAUGE_ERR_TOO_MANY_BLOBS, err);
}

static void test_errored_if_frame_size_mismatch(void) {
    g_bg.width = TU_IMAGE_WIDTH_MAX + 1;

    gauge_err_t err = gauge_calibrate_spin(&g_frame, &g_bg,
                                           GAUGE_BINARIZATION_THRESHOLD, &g_ca_data);

    TEST_ASSERT_EQUAL(GAUGE_ERR_FRAME_SIZE_MISMATCH, err);
}

static void test_errored_if_blob_not_found(void) {
    // frame == bg → subtraction → all zeros → no blob above threshold
    memset(g_frame_buf, 42, TU_FRAME_BUF_LEN); // NOLINT
    memset(g_bg_buf, 42, TU_FRAME_BUF_LEN);    // NOLINT

    gauge_err_t err = gauge_calibrate_spin(&g_frame, &g_bg,
                                           GAUGE_BINARIZATION_THRESHOLD, &g_ca_data);

    TEST_ASSERT_EQUAL(GAUGE_ERR_BLOB_NOT_FOUND, err);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_errored_if_arrow_displacement_too_small);
    RUN_TEST(test_determines_spin_when_arrow_displaced_enough);
    RUN_TEST(test_errored_if_frame_size_mismatch);
    RUN_TEST(test_errored_if_too_many_blobs);
    RUN_TEST(test_errored_if_blob_not_found);

    return UNITY_END();
}
