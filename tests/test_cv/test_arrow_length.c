#include "gauge.h"
#include "gauge/cv.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN];
static gauge_frame_t g_frame;

void setUp() {
    memset(g_frame_buf, 0, TU_FRAME_BUF_LEN);
    g_frame = (gauge_frame_t) {.buf = g_frame_buf,
                               .width = TU_IMAGE_WIDTH_MAX,
                               .height = TU_IMAGE_HEIGHT_MAX};
}

void tearDown() {}

static void test_arrow_length(const char *frame_path, gauge_pointf_t *pivot,
                              const char *name) {
    FIXTURES_LOAD_FRAME(frame_path, &g_frame);

    size_t arrow_len = gauge_cv_arrow_length(&g_frame, pivot);

    SNAPSHOT_ASSERT_SIZE_T(name, arrow_len);
}

#define CASES(X)                                                                    \
    X(test_arrow_length__set_1_00767591132, "set/1/00767591132_largest.json",       \
      ((gauge_pointf_t) {152.9689F, 98.83491F}))                                    \
    X(test_arrow_length__set_1_12756622577, "set/1/12756622577_largest.json",       \
      ((gauge_pointf_t) {152.9689F, 98.83491F}))

#define DEF_TEST(name, frame, PIVOT)                                                \
    static void name() {                                                            \
        gauge_pointf_t pivot = PIVOT;                                               \
        test_arrow_length(frame, &pivot, #name);                                    \
    }
CASES(DEF_TEST) // NOLINT(readability-magic-numbers)
#undef DEF_TEST

int main() {
    UNITY_BEGIN();

#define RUN(name, ...) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
