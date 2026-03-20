#include "gauge.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "unity.h"

#define MAX_FRAMES ((size_t) 32)

static tu_image_t g_imgs[MAX_FRAMES];
static gauge_frame_t g_frames[MAX_FRAMES];

void setUp(void) {}

void tearDown(void) {}

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

static void test_calibrate(const char *images_dir, const char *first_path,
                           const char *last_path, const char *name) {
    size_t count;
    FIXTURES_LOAD_IMAGES(images_dir, g_imgs, MAX_FRAMES, count);
    tu_to_frames(g_imgs, g_frames, count);

    gauge_calibration_data_t ca_data;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_calibrate(g_frames, count, &ca_data));

    SNAPSHOT_ASSERT_CALIBRATION(name, &ca_data);

    save_examples(first_path, last_path, &ca_data, name);
}

#define CASES(X)                                                                    \
    X(test_calibrate__set_1, "set/1", "set/1/00767591132.jpg",                      \
      "set/1/12756622577.jpg")

#define DEF_TEST(name, dir, first, last)                                            \
    static void name(void) {                                                        \
        test_calibrate(dir, first, last, #name);                                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

#define MINIMAL_FRAME_SIZE ((size_t) 4)
#define MINIMAL_FRAME_BUF_LEN (MINIMAL_FRAME_SIZE * MINIMAL_FRAME_SIZE)

static void test_errored_if_too_few_frames(void) {
    static uint8_t bufs[2][MINIMAL_FRAME_BUF_LEN];
    gauge_frame_t frames[2] = {
        {.buf = bufs[0],
         .buf_len = MINIMAL_FRAME_BUF_LEN,
         .width = MINIMAL_FRAME_SIZE,
         .height = MINIMAL_FRAME_SIZE},
        {.buf = bufs[1],
         .buf_len = MINIMAL_FRAME_BUF_LEN,
         .width = MINIMAL_FRAME_SIZE,
         .height = MINIMAL_FRAME_SIZE},
    };

    gauge_calibration_data_t ca_data;
    TEST_ASSERT_EQUAL(GAUGE_ERR_SPIN_UNDETERMINED,
                      gauge_calibrate(frames, 2, &ca_data));
}

static void test_errored_if_frame_sizes_mismatch(void) {
    static uint8_t bufs[3][MINIMAL_FRAME_BUF_LEN];
    gauge_frame_t frames[3] = {
        {.buf = bufs[0],
         .buf_len = MINIMAL_FRAME_BUF_LEN,
         .width = MINIMAL_FRAME_SIZE,
         .height = MINIMAL_FRAME_SIZE},
        {.buf = bufs[1],
         .buf_len = MINIMAL_FRAME_BUF_LEN,
         .width = MINIMAL_FRAME_SIZE,
         .height = MINIMAL_FRAME_SIZE},
        {.buf = bufs[2],
         .buf_len = MINIMAL_FRAME_BUF_LEN,
         .width = MINIMAL_FRAME_SIZE + 1,
         .height = MINIMAL_FRAME_SIZE},
    };

    gauge_calibration_data_t ca_data;
    TEST_ASSERT_EQUAL(GAUGE_ERR_FRAME_SIZE_MISMATCH,
                      gauge_calibrate(frames, 3, &ca_data));
}

int main(void) {
    UNITY_BEGIN();

#define RUN(name, dir, first, last) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_errored_if_too_few_frames);
    RUN_TEST(test_errored_if_frame_sizes_mismatch);

    return UNITY_END();
}
