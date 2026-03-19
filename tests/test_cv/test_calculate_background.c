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

#define CASES(X) X(test_calculate_background__set_1, "set/1")

#define DEF_TEST(name, dir)                                                         \
    static void name(void) {                                                        \
        test_calculate_background(dir, #name);                                      \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, dir) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
