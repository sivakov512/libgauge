#include "gauge/cv.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "unity.h"

static const float POINT_EPSILON = 0.5F;
static const float ORDER_EPSILON = 1e-5F;

static gauge_line_t g_line1;
static gauge_line_t g_line2;

void setUp(void) {}

void tearDown(void) {}

static void save_example_on_source(const char *name, const char *suffix,
                                   const char *source_image_path,
                                   const gauge_pointf_t *point) {
    tu_image_t img;
    FIXTURES_LOAD_IMAGE(source_image_path, &img);
    tu_draw_point(&img, point->x, point->y, TU_RED);

    char example_name[TU_EXAMPLE_NAME_LEN];
    (void) snprintf(example_name, sizeof(example_name), "%s_%s", name, suffix);
    EXAMPLES_SAVE_IMAGE(example_name, &img);
}

static void test_intersect_lines(const char *line1_path, const char *line2_path,
                                 const char *src1_path, const char *src2_path,
                                 const char *name) {
    FIXTURES_LOAD_LINE(line1_path, &g_line1);
    FIXTURES_LOAD_LINE(line2_path, &g_line2);

    gauge_pointf_t point;
    TEST_ASSERT_EQUAL(GAUGE_OK,
                      gauge_cv_intersect_lines(&g_line1, &g_line2, &point));

    SNAPSHOT_ASSERT_POINTF(name, &point, POINT_EPSILON);

    save_example_on_source(name, "on_first", src1_path, &point);
    save_example_on_source(name, "on_last", src2_path, &point);
}

#define CASES(X)                                                                    \
    X(test_intersect_lines__set_1, "set/1/00767591132_line.json",                   \
      "set/1/12756622577_line.json", "set/1/00767591132.jpg",                       \
      "set/1/12756622577.jpg")

#define DEF_TEST(name, l1, l2, src1, src2)                                          \
    static void name(void) {                                                        \
        test_intersect_lines(l1, l2, src1, src2, #name);                            \
    }
CASES(DEF_TEST)
#undef DEF_TEST

static void test_doesnt_depend_on_order() {
    FIXTURES_LOAD_LINE("set/1/00767591132_line.json", &g_line1);
    FIXTURES_LOAD_LINE("set/1/12756622577_line.json", &g_line2);

    gauge_pointf_t forward;
    gauge_pointf_t reversed;
    TEST_ASSERT_EQUAL(GAUGE_OK,
                      gauge_cv_intersect_lines(&g_line1, &g_line2, &forward));
    TEST_ASSERT_EQUAL(GAUGE_OK,
                      gauge_cv_intersect_lines(&g_line2, &g_line1, &reversed));

    TEST_ASSERT_EQUAL(forward.x, reversed.x);
    TEST_ASSERT_EQUAL(forward.y, reversed.y);
}

int main(void) {
    UNITY_BEGIN();

#define RUN(name, l1, l2, src1, src2) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_doesnt_depend_on_order);

    return UNITY_END();
}
