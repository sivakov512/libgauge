#include "gauge.h"
#include "tu/convert.h"
#include "tu/examples.h"
#include "tu/fixtures.h"
#include "tu/image.h"
#include "tu/snapshot.h"
#include "unity.h"

static const float ANGLE_EPSILON = 0.001F;

static tu_image_t g_img;
static gauge_frame_t g_frame;
static gauge_calibration_data_t g_ca_data;

void setUp() {}

void tearDown() {}

static void save_example(const char *img_path,
                         const gauge_calibration_data_t *ca_data, float angle,
                         const char *name) {
    tu_image_t img;
    FIXTURES_LOAD_IMAGE(img_path, &img);
    tu_draw_vector(&img, ca_data->pivot.x, ca_data->pivot.y, angle,
                   (float) ca_data->arrow_len, TU_RED);
    tu_draw_point(&img, ca_data->pivot.x, ca_data->pivot.y, TU_GREEN);
    EXAMPLES_SAVE_IMAGE(name, &img);
}

static void test_scan_radial(const char *img_path, const char *ca_data_path,
                             const char *name) {
    FIXTURES_LOAD_CALIBRATION(ca_data_path, &g_ca_data);
    FIXTURES_LOAD_IMAGE(img_path, &g_img);
    tu_to_frames(&g_img, &g_frame, 1);

    float angle;
    gauge_err_t err =
        gauge_scan_radial(&g_frame, &g_ca_data, GAUGE_RADIAL_SCAN_STEP, &angle);

    TEST_ASSERT_EQUAL(GAUGE_OK, err);
    SNAPSHOT_ASSERT_FLOAT(name, angle, ANGLE_EPSILON);

    save_example(img_path, &g_ca_data, angle, name);
}

// clang-format off
#define CASES(X) \
    X(test_scan_radial__set_1__00767591132,  "set/1/00767591132.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__01356622581,  "set/1/01356622581.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__01956622578,  "set/1/01956622578.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__02556622581,  "set/1/02556622581.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__03156622580,  "set/1/03156622580.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__03756622578,  "set/1/03756622578.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__04356622579,  "set/1/04356622579.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__04956622579,  "set/1/04956622579.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__05556622577,  "set/1/05556622577.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__06156622580,  "set/1/06156622580.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__06756622581,  "set/1/06756622581.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__07356622579,  "set/1/07356622579.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__07956622579,  "set/1/07956622579.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__08556622580,  "set/1/08556622580.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__09156622578,  "set/1/09156622578.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__09756622581,  "set/1/09756622581.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__10356622581,  "set/1/10356622581.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__10956622583,  "set/1/10956622583.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__11556622580,  "set/1/11556622580.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__12156622581,  "set/1/12156622581.jpg",  "set/1/ca_data.json") \
    X(test_scan_radial__set_1__12756622577,  "set/1/12756622577.jpg",  "set/1/ca_data.json")
// clang-format on

#define DEF_TEST(name, img, ca_data)                                                \
    static void name() {                                                            \
        test_scan_radial(img, ca_data, #name);                                      \
    }
CASES(DEF_TEST)
#undef DEF_TEST

static void test_errored_if_spin_unknown() {
    gauge_calibration_data_t ca_data = {.spin = GAUGE_SPIN_UNKNOWN};

    float angle;
    gauge_err_t err =
        gauge_scan_radial(&g_frame, &ca_data, GAUGE_RADIAL_SCAN_STEP, &angle);

    TEST_ASSERT_EQUAL(GAUGE_ERR_SPIN_UNDETERMINED, err);
}

int main() {
    UNITY_BEGIN();

#define RUN(name, ...) RUN_TEST(name);
    CASES(RUN)
#undef RUN

    RUN_TEST(test_errored_if_spin_unknown);

    return UNITY_END();
}
