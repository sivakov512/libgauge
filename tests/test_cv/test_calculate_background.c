#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"

static tu_image_t g_imgs[TU_MAX_FILES];
static gauge_frame_t g_frames[TU_MAX_FILES];
static uint8_t g_bg_buf[TU_FRAME_BUF_LEN] = {0};

void setUp(void) {}

void tearDown(void) {}

static void test_calculate_background(const char *source_dir, const char *name) {
    size_t count;
    FIXTURES_LOAD_IMAGES(source_dir, g_imgs, TU_MAX_FILES, count);
    tu_to_frames(g_imgs, g_frames, count);

    gauge_frame_t background = {.buf = g_bg_buf,
                                .buf_len = g_frames[0].buf_len,
                                .width = g_frames[0].width,
                                .height = g_frames[0].height};
    gauge_cv_calculate_background(g_frames, count, &background);

    SNAPSHOT_ASSERT_FRAME(name, &background);

    EXAMPLES_SAVE_FRAME(name, &background);
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
