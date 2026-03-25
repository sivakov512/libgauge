#include "gauge/cv.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"
#include <string.h>

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_frame;

void setUp() {
    memset(g_frame_buf, 0, TU_FRAME_BUF_LEN);
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .width = TU_IMAGE_WIDTH_MAX,
                               .height = TU_IMAGE_HEIGHT_MAX};
}

void tearDown() {}

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
    static void name() {                                                            \
        test_binarize(frame, #name);                                                \
    }
CASES(DEF_TEST)
#undef DEF_TEST

int main() {
    UNITY_BEGIN();

#define RUN(name, ...) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
