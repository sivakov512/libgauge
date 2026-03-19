#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "tu/utils.h"
#include "unity.h"

#include <stdint.h>

void setUp(void) {}

void tearDown(void) {}

#define BINARIZE_THRESHOLD 20

static void test_binarize(const char *frame_path, const char *name) {
    static uint8_t frame_buf[TU_FRAME_BUF_LEN];
    static gauge_frame_t frame;
    TEST_ASSERT_TRUE(
        TU_FIXTURES_LOAD_FRAME(frame_path, frame_buf, TU_FRAME_BUF_LEN, &frame));

    gauge_cv_binarize(&frame, BINARIZE_THRESHOLD);

    TU_SNAPSHOT_ASSERT_FRAME(name, &frame);

    tu_unbinarize(&frame);
    TU_EXAMPLES_SAVE_FRAME(name, &frame);
}

/* clang-format off */
#define CASES(X) \
    X(test_binarize__set_1_00767591132, "set/1/00767591132_subtracted.json") \
    X(test_binarize__set_1_12756622577, "set/1/12756622577_subtracted.json")
/* clang-format on */

#define DEF_TEST(name, frame)                                                       \
    static void name(void) {                                                        \
        test_binarize(frame, #name);                                                \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, frame) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
