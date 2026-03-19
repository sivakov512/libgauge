#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "tu/utils.h"
#include "unity.h"

#include <stdint.h>
#include <stdio.h>

#define EXAMPLE_NAME_LEN ((size_t) 128)

void setUp(void) {}

void tearDown(void) {}

static void draw_line_with_origin(tu_image_t *img, const gauge_line_t *line) {
    tu_draw_line(img, line, TU_RED);
    tu_draw_point(img, line->origin.x, line->origin.y, TU_GREEN);
}

static void save_example(const char *name, const char *suffix, tu_image_t *img) {
    char example_name[EXAMPLE_NAME_LEN];
    (void) snprintf(example_name, sizeof(example_name), "%s_%s", name, suffix);
    TU_EXAMPLES_SAVE_IMAGE(example_name, img);
}

static void test_blob_to_line(const char *frame_path, const char *source_image_path,
                              const char *name) {
    static uint8_t frame_buf[TU_FRAME_BUF_LEN];
    static gauge_frame_t frame;
    TEST_ASSERT_TRUE(
        TU_FIXTURES_LOAD_FRAME(frame_path, frame_buf, TU_FRAME_BUF_LEN, &frame));

    gauge_line_t line;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_blob_to_line(&frame, &line));

    TU_SNAPSHOT_ASSERT_LINE(name, &line);

    tu_unbinarize(&frame);

    static tu_image_t on_largest;
    tu_from_frame(&frame, &on_largest);
    draw_line_with_origin(&on_largest, &line);
    save_example(name, "on_largest", &on_largest);

    tu_image_t source_img;
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_IMAGE(source_image_path, &source_img));
    draw_line_with_origin(&source_img, &line);
    save_example(name, "on_source", &source_img);
}

/* clang-format off */
#define CASES(X) \
    X(test_blob_to_line__set_1_00767591132, "set/1/00767591132_largest.json", "set/1/00767591132.jpg") \
    X(test_blob_to_line__set_1_12756622577, "set/1/12756622577_largest.json", "set/1/12756622577.jpg")
/* clang-format on */

#define DEF_TEST(name, frame, source)                                               \
    static void name(void) {                                                        \
        test_blob_to_line(frame, source, #name);                                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, frame, source) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
