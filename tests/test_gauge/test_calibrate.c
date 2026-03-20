#include "gauge.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "unity.h"

#include <stdint.h>
#include <stdio.h>

#define MAX_FRAMES ((size_t) 32)
#define EXAMPLE_NAME_LEN ((size_t) 128)

void setUp(void) {}

void tearDown(void) {}

static void save_example_on(const char *img_path,
                            const gauge_calibration_data_t *ca_data,
                            const char *example_name) {
    tu_image_t img;
    if (!TU_FIXTURES_LOAD_IMAGE(img_path, &img)) {
        return;
    }
    tu_draw_calibration(&img, ca_data);
    TU_EXAMPLES_SAVE_IMAGE(example_name, &img);
}

static void save_examples(const char *first_path, const char *last_path,
                          const gauge_calibration_data_t *ca_data,
                          const char *name) {
    char example_name[EXAMPLE_NAME_LEN];

    (void) snprintf(example_name, sizeof(example_name), "%s_on_first", name);
    save_example_on(first_path, ca_data, example_name);

    (void) snprintf(example_name, sizeof(example_name), "%s_on_last", name);
    save_example_on(last_path, ca_data, example_name);
}

static void test_calibrate(const char *images_dir, const char *first_path,
                           const char *last_path, const char *name) {
    static tu_image_t imgs[MAX_FRAMES];
    size_t count = TU_FIXTURES_LOAD_IMAGES(images_dir, imgs, MAX_FRAMES);
    TEST_ASSERT_GREATER_THAN(0, count);

    static gauge_frame_t frames[MAX_FRAMES];
    tu_to_frames(imgs, frames, count);

    gauge_calibration_data_t ca_data;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_calibrate(frames, count, &ca_data));

    TU_SNAPSHOT_ASSERT_CALIBRATION(name, &ca_data);

    save_examples(first_path, last_path, &ca_data, name);
}

/* clang-format off */
#define CASES(X) \
    X(test_calibrate__set_1,        \
      "set/1",                      \
      "set/1/00767591132.jpg",      \
      "set/1/12756622577.jpg")
/* clang-format on */

#define DEF_TEST(name, dir, first, last)                                            \
    static void name(void) {                                                        \
        test_calibrate(dir, first, last, #name);                                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, dir, first, last) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
