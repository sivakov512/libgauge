#include "tu/snapshot.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

#define FRAME_WIDTH 3U
#define FRAME_HEIGHT 2U
#define FRAME_PIXELS ((size_t) (FRAME_WIDTH * FRAME_HEIGHT))

static const uint8_t FRAME_BUF[] = {10, 20, 30, 40, 50, 60};
static const float FLOAT_VALUE = 1.5F;
static const float FLOAT_EPSILON = 0.0001F;

static void test_snapshot_frame(void) {
    static uint8_t buf[FRAME_PIXELS];
    memcpy(buf, FRAME_BUF, sizeof(buf));

    gauge_frame_t frame = {.buf = buf,
                           .buf_len = FRAME_PIXELS,
                           .width = FRAME_WIDTH,
                           .height = FRAME_HEIGHT};

    SNAPSHOT_ASSERT_FRAME("test_snapshot_frame_frame", &frame);
}

static void test_snapshot_float(void) {
    SNAPSHOT_ASSERT_FLOAT("test_snapshot_float_float", FLOAT_VALUE, FLOAT_EPSILON);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_snapshot_frame);
    RUN_TEST(test_snapshot_float);

    return UNITY_END();
}
