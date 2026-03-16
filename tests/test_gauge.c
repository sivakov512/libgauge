#include "gauge/cv.h"
#include "unity.h"
#include "utils.h"

void setUp(void) {}

void tearDown(void) {}

static void test_min_blob_area(void) {
    TEST_ASSERT_EQUAL_INT(utils_example(), GAUGE_CV_EXAMPLE);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_min_blob_area);

    return UNITY_END();
}
