#include "gauge/cv.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "tu/utils.h"
#include "unity.h"

#include <stdint.h>
#include <stdio.h>

#define EXAMPLE_NAME_LEN ((size_t) 128)
static const float POINT_EPSILON = 0.5F;
static const float ORDER_EPSILON = 1e-5F;

void setUp(void) {}

void tearDown(void) {}

static void save_example_on_source(const char *name, const char *suffix,
                                   const char *source_image_path,
                                   const gauge_pointf_t *point) {
    tu_image_t img;
    if (!TU_FIXTURES_LOAD_IMAGE(source_image_path, &img)) {
        return;
    }
    tu_draw_point(&img, point->x, point->y, TU_RED);

    char example_name[EXAMPLE_NAME_LEN];
    (void) snprintf(example_name, sizeof(example_name), "%s_%s", name, suffix);
    TU_EXAMPLES_SAVE_IMAGE(example_name, &img);
}

static void test_intersect_lines(const char *line1_path, const char *line2_path,
                                 const char *src1_path, const char *src2_path,
                                 const char *name) {
    gauge_line_t line1;
    gauge_line_t line2;
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_LINE(line1_path, &line1));
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_LINE(line2_path, &line2));

    gauge_pointf_t point;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_intersect_lines(&line1, &line2, &point));

    char snap_x[EXAMPLE_NAME_LEN];
    char snap_y[EXAMPLE_NAME_LEN];
    (void) snprintf(snap_x, sizeof(snap_x), "%s_x", name);
    (void) snprintf(snap_y, sizeof(snap_y), "%s_y", name);
    TU_SNAPSHOT_ASSERT_FLOAT(snap_x, point.x, POINT_EPSILON);
    TU_SNAPSHOT_ASSERT_FLOAT(snap_y, point.y, POINT_EPSILON);

    save_example_on_source(name, "on_0", src1_path, &point);
    save_example_on_source(name, "on_1", src2_path, &point);
}

static void test_intersect_lines__order_independent(const char *line1_path,
                                                    const char *line2_path) {
    gauge_line_t line1;
    gauge_line_t line2;
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_LINE(line1_path, &line1));
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_LINE(line2_path, &line2));

    gauge_pointf_t forward;
    gauge_pointf_t reversed;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_intersect_lines(&line1, &line2, &forward));
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_intersect_lines(&line2, &line1, &reversed));

    TEST_ASSERT_FLOAT_WITHIN(ORDER_EPSILON, forward.x, reversed.x);
    TEST_ASSERT_FLOAT_WITHIN(ORDER_EPSILON, forward.y, reversed.y);
}

/* clang-format off */
#define CASES(X) \
    X(test_intersect_lines__set_1,                          \
      "set/1/00767591132_line.json",                        \
      "set/1/12756622577_line.json",                        \
      "set/1/00767591132.jpg",                              \
      "set/1/12756622577.jpg")
/* clang-format on */

#define DEF_TEST(name, l1, l2, src1, src2)                                          \
    static void name(void) {                                                        \
        test_intersect_lines(l1, l2, src1, src2, #name);                            \
    }
CASES(DEF_TEST)
#undef DEF_TEST

static void test_intersect_lines__set_1__order_independent(void) {
    test_intersect_lines__order_independent("set/1/00767591132_line.json",
                                            "set/1/12756622577_line.json");
}

int main(void) {
    UNITY_BEGIN();

#define RUN(name, l1, l2, src1, src2) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_intersect_lines__set_1__order_independent);

    return UNITY_END();
}
