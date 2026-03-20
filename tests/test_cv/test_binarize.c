#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN] = {0};
static gauge_frame_t g_frame = {.buf = g_frame_buf, .buf_len = TU_FRAME_BUF_LEN};

void setUp(void) {}

void tearDown(void) {}

static void test_binarize(const char *frame_path, const char *name) {
    FIXTURES_LOAD_FRAME(frame_path, &g_frame);

    gauge_cv_binarize(&g_frame, GAUGE_BINARIZATION_THRESHOLD);

    SNAPSHOT_ASSERT_FRAME(name, &g_frame);

    tu_unbinarize(&g_frame);
    EXAMPLES_SAVE_FRAME(name, &g_frame);
}

#define CASES(X)                                                                    \
    X(test_binarize__set_1_00767591132, "set/1/00767591132_subtracted.json")        \
    X(test_binarize__set_1_12756622577, "set/1/12756622577_subtracted.json")

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
