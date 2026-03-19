#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "tu/utils.h"
#include "unity.h"

#include <stdint.h>

void setUp(void) {}

void tearDown(void) {}

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

/* clang-format off */
#define CASES(X) \
    X(test_subtract_background__set_1_00767591132, "set/1/00767591132.jpg", "set/1/background.json") \
    X(test_subtract_background__set_1_12756622577, "set/1/12756622577.jpg", "set/1/background.json")
/* clang-format on */

#define DEF_TEST(name, image, bg)                                                   \
    static void name(void) {                                                        \
        test_subtract_background(image, bg, #name);                                 \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, image, bg) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
