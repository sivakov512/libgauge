#include "gauge.h"
#include "gauge/cv.h"
#include "tu/fixtures.h"
#include "tu/snapshot.h"
#include "unity.h"

static uint8_t g_frame_buf[TU_FRAME_BUF_LEN] = {0};
static gauge_frame_t g_frame = {.buf = g_frame_buf, .buf_len = TU_FRAME_BUF_LEN};

void setUp(void) {}

void tearDown(void) {}

static void test_arrow_length(const char *frame_path, gauge_pointf_t *pivot,
                              const char *name) {
    FIXTURES_LOAD_FRAME(frame_path, &g_frame);

    size_t arrow_len = gauge_cv_arrow_length(&g_frame, pivot);

    SNAPSHOT_ASSERT_SIZE_T(name, arrow_len);
}

#define CASES(X)                                                                    \
    X(test_arrow_length__set_1_00767591132, "set/1/00767591132_largest.json",       \
      ((gauge_pointf_t) {152.96891720365855F, 98.83491199663723F}))                 \
    X(test_arrow_length__set_1_12756622577, "set/1/12756622577_largest.json",       \
      ((gauge_pointf_t) {152.96891720365855F, 98.83491199663723F}))

#define DEF_TEST(name, frame, PIVOT)                                                \
    static void name(void) {                                                        \
        gauge_pointf_t pivot = PIVOT;                                               \
        test_arrow_length(frame, &pivot, #name);                                    \
    }
CASES(DEF_TEST) // NOLINT(readability-magic-numbers)
#undef DEF_TEST

int main(void) {
    UNITY_BEGIN();

#define RUN(name, frame, PIVOT) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
