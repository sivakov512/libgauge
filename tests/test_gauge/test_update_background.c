#include "gauge.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"
#include <string.h>

static tu_image_t g_imgs[TU_MAX_FILES];
static gauge_frame_t g_frames[TU_MAX_FILES];

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_frame;

static uint8_t g_bg_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_bg;

void setUp(void) {
    memset(g_frame_buf, 0, sizeof(g_frame_buf));
    g_frame = (gauge_frame_t) {
        .buf = g_frame_buf,
        .buf_len = TU_FRAME_BUF_LEN,
        .width = TU_IMAGE_WIDTH_MAX,
        .height = TU_IMAGE_HEIGHT_MAX,
    };

    memset(g_bg_buf, 0, sizeof(g_bg_buf));
    g_bg = (gauge_frame_t) {
        .buf = g_bg_buf,
        .buf_len = TU_FRAME_BUF_LEN,
        .width = TU_IMAGE_WIDTH_MAX,
        .height = TU_IMAGE_HEIGHT_MAX,
    };
}

void tearDown(void) {}

static void test_update_background(const char *dir, const char *name) {
    size_t count;
    FIXTURES_LOAD_IMAGES(dir, g_imgs, TU_MAX_FILES, count);
    tu_to_frames(g_imgs, g_frames, count);

    for (size_t i = 0; i < count; ++i) {
        gauge_err_t err = gauge_update_background(&g_frames[i], &g_bg);
        TEST_ASSERT_EQUAL(GAUGE_OK, err);
    }

    SNAPSHOT_ASSERT_FRAME(name, &g_bg);

    EXAMPLES_SAVE_FRAME(name, &g_bg);
}

#define CASES(X) X(test_update_background__set_1, "set/1")

#define DEF_TEST(name, dir)                                                         \
    static void name(void) {                                                        \
        test_update_background(dir, #name);                                         \
    }
CASES(DEF_TEST)
#undef DEF_TEST

static void test_errored_if_frame_size_mismatch(void) {
    g_bg.width = TU_IMAGE_WIDTH_MAX + 1;

    gauge_err_t err = gauge_update_background(&g_frame, &g_bg);

    TEST_ASSERT_EQUAL(GAUGE_ERR_FRAME_SIZE_MISMATCH, err);
}

static void test_errored_if_buf_len_mismatch(void) {
    g_bg.buf_len = TU_FRAME_BUF_LEN - 1;

    gauge_err_t err = gauge_update_background(&g_frame, &g_bg);

    TEST_ASSERT_EQUAL(GAUGE_ERR_FRAME_SIZE_MISMATCH, err);
}

int main(void) {
    UNITY_BEGIN();

#define RUN(name, ...) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_errored_if_frame_size_mismatch);
    RUN_TEST(test_errored_if_buf_len_mismatch);

    return UNITY_END();
}
