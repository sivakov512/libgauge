#include "tu/json.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

#define FRAME_WIDTH 3U
#define FRAME_HEIGHT 2U
#define FRAME_PIXELS ((size_t) (FRAME_WIDTH * FRAME_HEIGHT))

static const uint8_t FRAME_BUF[] = {10, 20, 30, 40, 50, 60};
static const float FLOAT_VALUE = 3.14F;
static const float FLOAT_EPSILON = 0.0001F;

static const float LINE_ORIGIN_X = 10.5F;
static const float LINE_ORIGIN_Y = 20.5F;
static const float LINE_DIRECTION_X = 30.0F;
static const float LINE_DIRECTION_Y = 40.0F;
static const float LINE_FLOAT_EPSILON = 1e-5F;

static const size_t CA_PIVOT_X = 100;
static const size_t CA_PIVOT_Y = 200;
static const float CA_ANGLE_START = 1.23F;
static const float CA_ANGLE_END = 4.56F;
static const size_t CA_ARROW_LEN = 50;

static void test_frame_roundtrip(void) {
    static uint8_t src_buf[FRAME_PIXELS];
    memcpy(src_buf, FRAME_BUF, sizeof(src_buf));

    gauge_frame_t src = {.buf = src_buf,
                         .buf_len = FRAME_PIXELS,
                         .width = FRAME_WIDTH,
                         .height = FRAME_HEIGHT};

    const char *path = TEST_EXAMPLES_DIR "/test_frame_roundtrip.json";
    TEST_ASSERT_TRUE(tu_json_write_frame(path, &src));

    static uint8_t dst_buf[FRAME_PIXELS];
    gauge_frame_t dst = {.buf = dst_buf, .buf_len = sizeof(dst_buf)};
    TEST_ASSERT_TRUE(tu_json_read_frame(path, &dst));

    TEST_ASSERT_EQUAL(src.width, dst.width);
    TEST_ASSERT_EQUAL(src.height, dst.height);
    TEST_ASSERT_EQUAL(src.buf_len, dst.buf_len);
    TEST_ASSERT_EQUAL_MEMORY(src.buf, dst.buf, src.buf_len);
}

static void test_line_roundtrip(void) {
    gauge_line_t src = {.origin = {.x = LINE_ORIGIN_X, .y = LINE_ORIGIN_Y},
                        .direction = {.x = LINE_DIRECTION_X, .y = LINE_DIRECTION_Y}};

    const char *path = TEST_EXAMPLES_DIR "/test_line_roundtrip.json";
    TEST_ASSERT_TRUE(tu_json_write_line(path, &src));

    gauge_line_t dst;
    TEST_ASSERT_TRUE(tu_json_read_line(path, &dst));

    TEST_ASSERT_FLOAT_WITHIN(LINE_FLOAT_EPSILON, src.origin.x, dst.origin.x);
    TEST_ASSERT_FLOAT_WITHIN(LINE_FLOAT_EPSILON, src.origin.y, dst.origin.y);
    TEST_ASSERT_FLOAT_WITHIN(LINE_FLOAT_EPSILON, src.direction.x, dst.direction.x);
    TEST_ASSERT_FLOAT_WITHIN(LINE_FLOAT_EPSILON, src.direction.y, dst.direction.y);
}

static void test_calibration_roundtrip(void) {
    gauge_calibration_data_t src = {.pivot = {.x = CA_PIVOT_X, .y = CA_PIVOT_Y},
                                    .angle_start_rad = CA_ANGLE_START,
                                    .angle_end_rad = CA_ANGLE_END,
                                    .spin = GAUGE_SPIN_CW,
                                    .arrow_len = CA_ARROW_LEN};

    const char *path = TEST_EXAMPLES_DIR "/test_calibration_roundtrip.json";
    TEST_ASSERT_TRUE(tu_json_write_calibration(path, &src));

    gauge_calibration_data_t dst;
    TEST_ASSERT_TRUE(tu_json_read_calibration(path, &dst));

    TEST_ASSERT_EQUAL(src.pivot.x, dst.pivot.x);
    TEST_ASSERT_EQUAL(src.pivot.y, dst.pivot.y);
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_EPSILON, src.angle_start_rad,
                             dst.angle_start_rad);
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_EPSILON, src.angle_end_rad, dst.angle_end_rad);
    TEST_ASSERT_EQUAL_INT8(src.spin, dst.spin);
    TEST_ASSERT_EQUAL(src.arrow_len, dst.arrow_len);
}

static void test_float_roundtrip(void) {
    const char *path = TEST_EXAMPLES_DIR "/test_float_roundtrip.json";
    TEST_ASSERT_TRUE(tu_json_write_float(path, FLOAT_VALUE));

    float result;
    TEST_ASSERT_TRUE(tu_json_read_float(path, &result));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_EPSILON, FLOAT_VALUE, result);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_frame_roundtrip);
    RUN_TEST(test_line_roundtrip);
    RUN_TEST(test_calibration_roundtrip);
    RUN_TEST(test_float_roundtrip);

    return UNITY_END();
}
