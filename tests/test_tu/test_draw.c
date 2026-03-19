#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

static const float PIVOT_X = 160.0F;
static const float PIVOT_Y = 120.0F;
static const float ANGLE = 0.5F;
static const float ARROW_LEN = 80.0F;

static void test_draw_vector_and_point(void) {
    tu_image_t img;
    TEST_ASSERT_TRUE(TU_FIXTURES_LOAD_IMAGE("common/sample_1.jpg", &img));

    tu_draw_vector(&img, PIVOT_X, PIVOT_Y, ANGLE, ARROW_LEN, TU_RED);
    tu_draw_point(&img, PIVOT_X, PIVOT_Y, TU_GREEN);

    TU_EXAMPLES_SAVE_IMAGE("test_draw_vector_and_point", &img);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_draw_vector_and_point);

    return UNITY_END();
}
