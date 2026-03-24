#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

static const size_t SAMPLE_WIDTH = 320;
static const size_t SAMPLE_HEIGHT = 240;

static void test_to_frame() {
    tu_image_t img;
    FIXTURES_LOAD_IMAGE("common/sample_1.jpg", &img);

    gauge_frame_t frame;
    tu_to_frames(&img, &frame, 1);

    TEST_ASSERT_EQUAL(SAMPLE_WIDTH, frame.width);
    TEST_ASSERT_EQUAL(SAMPLE_HEIGHT, frame.height);
    TEST_ASSERT_EQUAL(SAMPLE_WIDTH * SAMPLE_HEIGHT, frame.buf_len);

    EXAMPLES_SAVE_FRAME("test_to_frame", &frame);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_to_frame);

    return UNITY_END();
}
