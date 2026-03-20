#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"

static tu_image_t g_img;

static gauge_frame_t g_frame;

static uint8_t g_bg_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_background = {.buf = g_bg_buf, .buf_len = TU_FRAME_BUF_LEN};

void setUp(void) {}

void tearDown(void) {}

static void test_subtract_background(const char *image_path, const char *bg_path,
                                     const char *name) {
    FIXTURES_LOAD_IMAGE(image_path, &g_img);
    tu_to_frames(&g_img, &g_frame, 1);
    FIXTURES_LOAD_FRAME(bg_path, &g_background);

    gauge_cv_subtract_background(&g_frame, &g_background);

    SNAPSHOT_ASSERT_FRAME(name, &g_frame);

    EXAMPLES_SAVE_FRAME(name, &g_frame);
}

#define CASES(X)                                                                    \
    X(test_subtract_background__set_1_00767591132, "set/1/00767591132.jpg",         \
      "set/1/background.json")                                                      \
    X(test_subtract_background__set_1_12756622577, "set/1/12756622577.jpg",         \
      "set/1/background.json")

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
