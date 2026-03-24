#include "gauge.h"
#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "unity.h"

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN] = {0};
static gauge_frame_t g_frame = {.buf = g_frame_buf, .buf_len = TU_FRAME_BUF_LEN};

void setUp() {}

void tearDown() {}

static void make_example(const char *name, const char *suffix, tu_image_t *img,
                         gauge_line_t *line) {
    tu_draw_line(img, line, TU_RED);
    tu_draw_point(img, line->origin.x, line->origin.y, TU_GREEN);

    char example_name[TU_EXAMPLE_NAME_LEN];
    (void) snprintf(example_name, sizeof(example_name), "%s_%s", name, suffix);
    EXAMPLES_SAVE_IMAGE(example_name, img);
}

static void test_blob_to_line(const char *frame_path, const char *source_image_path,
                              const char *name) {
    FIXTURES_LOAD_FRAME(frame_path, &g_frame);

    gauge_line_t line;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_blob_to_line(&g_frame, &line));

    SNAPSHOT_ASSERT_LINE(name, &line);

    {
        tu_unbinarize(&g_frame);
        static tu_image_t example;

        tu_from_frame(&g_frame, &example);
        make_example(name, "on_largest", &example, &line);

        FIXTURES_LOAD_IMAGE(source_image_path, &example);
        make_example(name, "on_source", &example, &line);
    }
}

#define CASES(X)                                                                    \
    X(test_blob_to_line__set_1_00767591132, "set/1/00767591132_largest.json",       \
      "set/1/00767591132.jpg")                                                      \
    X(test_blob_to_line__set_1_12756622577, "set/1/12756622577_largest.json",       \
      "set/1/12756622577.jpg")

#define DEF_TEST(name, frame, source)                                               \
    static void name() {                                                            \
        test_blob_to_line(frame, source, #name);                                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main() {
    UNITY_BEGIN();

#define RUN(name, frame, source) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
