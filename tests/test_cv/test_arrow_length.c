#include "gauge/cv.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"

#include <stdint.h>

static const float ARROW_LEN_EPSILON = 1.0F;

void setUp(void) {}

void tearDown(void) {}

static void test_arrow_length(const char *frame_path, const char *line1_path,
                              const char *line2_path, const char *name) {
    static uint8_t frame_buf[TU_FRAME_BUF_LEN];
    static gauge_frame_t frame;
    TEST_ASSERT_TRUE(
        TU_FIXTURES_LOAD_FRAME(frame_path, frame_buf, TU_FRAME_BUF_LEN, &frame));

    gauge_line_t line1;
    gauge_line_t line2;
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_LINE(line1_path, &line1));
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_LINE(line2_path, &line2));

    gauge_pointf_t pivot;
    TEST_ASSERT_EQUAL(GAUGE_OK, gauge_cv_intersect_lines(&line1, &line2, &pivot));

    float arrow_len = gauge_cv_arrow_length(&frame, &pivot);

    TU_SNAPSHOT_ASSERT_FLOAT(name, arrow_len, ARROW_LEN_EPSILON);
}

/* clang-format off */
#define CASES(X) \
    X(test_arrow_length__set_1_00767591132,             \
      "set/1/00767591132_largest.json",                 \
      "set/1/00767591132_line.json",                    \
      "set/1/12756622577_line.json")                    \
    X(test_arrow_length__set_1_12756622577,             \
      "set/1/12756622577_largest.json",                 \
      "set/1/00767591132_line.json",                    \
      "set/1/12756622577_line.json")
/* clang-format on */

#define DEF_TEST(name, frame, l1, l2)                                               \
    static void name(void) {                                                        \
        test_arrow_length(frame, l1, l2, #name);                                    \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, frame, l1, l2) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
