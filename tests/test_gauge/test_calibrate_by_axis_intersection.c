#include "gauge.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "tu/utils.h"
#include "unity.h"
#include <string.h>

#define MAX_FRAMES ((size_t) 32)

static uint8_t g_first_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_first;

static uint8_t g_last_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_last;

static uint8_t g_bg_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_bg;

static tu_image_t g_imgs[MAX_FRAMES];
static gauge_frame_t g_frames[MAX_FRAMES];

void setUp() {
    memset(g_bg_buf, 0, sizeof(g_bg_buf));

    g_first = (gauge_frame_t) {
        .buf = g_first_buf,
        .buf_len = TU_FRAME_BUF_LEN,
        .width = TU_IMAGE_WIDTH_MAX,
        .height = TU_IMAGE_HEIGHT_MAX,
    };
    g_last = (gauge_frame_t) {
        .buf = g_last_buf,
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

static void save_example_on(const char *img_path,
                            const gauge_calibration_data_t *ca_data,
                            const char *example_name) {
    tu_image_t img;
    FIXTURES_LOAD_IMAGE(img_path, &img);
    tu_draw_calibration(&img, ca_data);
    EXAMPLES_SAVE_IMAGE(example_name, &img);
}

static void save_examples(const char *first_path, const char *last_path,
                          const gauge_calibration_data_t *ca_data,
                          const char *name) {
    char example_name[TU_EXAMPLE_NAME_LEN];

    (void) snprintf(example_name, sizeof(example_name), "%s_on_first", name);
    save_example_on(first_path, ca_data, example_name);

    (void) snprintf(example_name, sizeof(example_name), "%s_on_last", name);
    save_example_on(last_path, ca_data, example_name);
}

static void test_calibrate_by_axis_intersection(const char *first_img_path,
                                                const char *last_img_path,
                                                const char *name) {
    FIXTURES_LOAD_FRAME("set/1/background.json", &g_bg);

    FIXTURES_LOAD_IMAGE(first_img_path, &g_imgs[0]);
    tu_to_frames(&g_imgs[0], &g_first, 1);

    FIXTURES_LOAD_IMAGE(last_img_path, &g_imgs[1]);
    tu_to_frames(&g_imgs[1], &g_last, 1);

    gauge_calibration_data_t ca_data;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_calibrate_by_axis_intersection(
                                    &g_first, &g_last, &g_bg,
                                    GAUGE_BINARIZATION_THRESHOLD, &ca_data));

    TEST_ASSERT_EQUAL(GAUGE_SPIN_UNKNOWN, ca_data.spin);
    SNAPSHOT_ASSERT_CALIBRATION(name, &ca_data);

    save_examples(first_img_path, last_img_path, &ca_data, name);
}

#define CASES(X)                                                                    \
    X(test_calibrate_by_axis_intersection__set_1, "set/1/00767591132.jpg",          \
      "set/1/12756622577.jpg")

#define DEF_TEST(name, first, last)                                                 \
    static void name() {                                                            \
        test_calibrate_by_axis_intersection(first, last, #name);                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

static void test_errored_if_frame_size_mismatch() {
    g_bg.width = TU_IMAGE_WIDTH_MAX + 1;
    gauge_calibration_data_t ca_data;

    gauge_err_t err = gauge_calibrate_by_axis_intersection(
        &g_first, &g_last, &g_bg, GAUGE_BINARIZATION_THRESHOLD, &ca_data);

    TEST_ASSERT_EQUAL(GAUGE_ERR_FRAME_SIZE_MISMATCH, err);
}

int main() {
    UNITY_BEGIN();

#define RUN(name, ...) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_errored_if_frame_size_mismatch);

    return UNITY_END();
}
