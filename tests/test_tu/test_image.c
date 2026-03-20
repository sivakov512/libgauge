#include "tu/examples.h"
#include "tu/fixtures.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

static const size_t SAMPLE_WIDTH = 320;
static const size_t SAMPLE_HEIGHT = 240;

static void test_load(void) {
    tu_image_t img;

    FIXTURES_LOAD_IMAGE("common/sample_1.jpg", &img);
    TEST_ASSERT_EQUAL(SAMPLE_WIDTH, img.width);
    TEST_ASSERT_EQUAL(SAMPLE_HEIGHT, img.height);

    EXAMPLES_SAVE_IMAGE("test_load", &img);
}

static void test_load_dir(void) {
    static tu_image_t imgs[TU_MAX_FILES];

    size_t count;
    FIXTURES_LOAD_IMAGES("common", imgs, TU_MAX_FILES, count);
    TEST_ASSERT_EQUAL(1, count);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_load);
    RUN_TEST(test_load_dir);

    return UNITY_END();
}
