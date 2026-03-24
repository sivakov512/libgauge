#include "gauge/utils.h"
#include "unity.h"

static const float EPSILON = 1e-5F;

void setUp() {}

void tearDown() {}

static void test_normalize_angle(float angle_rad, gauge_spin_t direction,
                                 float expected) {
    float got = gauge_utils_normalize_angle(angle_rad, direction);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, expected, got);
}

#define TWO_PI 6.283185F // NOLINT(readability-magic-numbers)

// clang-format off
#define CASES(X) \
    X(no_direction__angle_in_range__unchanged,                      1.0F,   0,  1.0F)           \
    X(no_direction__angle_above_pi__wrapped_into_range,             4.0F,   0,  4.0F - TWO_PI)  \
    X(no_direction__angle_below_minus_pi__wrapped_into_range,       -4.0F,  0,  -4.0F + TWO_PI) \
    X(cw__positive_angle__unchanged,                                1.0F,   1,  1.0F)           \
    X(cw__negative_angle__shifted_positive_for_boundary_crossing,   -1.0F,  1,  -1.0F + TWO_PI) \
    X(ccw__negative_angle__unchanged,                               -1.0F,  -1, -1.0F)          \
    X(ccw__positive_angle__shifted_negative_for_boundary_crossing,  1.0F,   -1, 1.0F - TWO_PI)
// clang-format on

#define DEF_TEST(name, angle, dir, expected)                                        \
    static void test_##name() {                                                     \
        test_normalize_angle((angle), (gauge_spin_t) (dir), (expected));            \
    }
CASES(DEF_TEST) // NOLINT(readability-magic-numbers)
#undef DEF_TEST

int main() {
    UNITY_BEGIN();

#define RUN(name, ...) RUN_TEST(test_##name);
    CASES(RUN)
#undef RUN

    return UNITY_END();
}
