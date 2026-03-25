# libgauge

[![Checks](https://github.com/sivakov512/libgauge/actions/workflows/checks.yml/badge.svg)](https://github.com/sivakov512/libgauge/actions/workflows/checks.yml)
[![Formatting](https://github.com/sivakov512/libgauge/actions/workflows/formatting.yml/badge.svg)](https://github.com/sivakov512/libgauge/actions/workflows/formatting.yml)

C library for reading analog gauge values from video frames. Designed for constrained environments — no OpenCV, no heavy dependencies. Works on embedded systems with a camera.

Python reference implementation used for algorithm prototyping: [sivakov512/libgauge-reference](https://github.com/sivakov512/libgauge-reference).

## Requirements

- CMake 3.16+
- C99 compiler
- `libm` (standard math library)

## Integration

<!--x-release-please-start-version-->
```cmake
include(FetchContent)

FetchContent_Declare(
    gauge
    GIT_REPOSITORY https://github.com/sivakov512/libgauge.git
    GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(gauge)

target_link_libraries(your_target PRIVATE gauge)
```
<!--x-release-please-end-->

Include the headers you need:

```c
#include "gauge.h"       // calibration and measurement
#include "gauge/cv.h"    // computer vision primitives
#include "gauge/utils.h" // angle utilities
```

## Usage

### 1. Prepare frames

Frames are grayscale, row-major. You allocate the buffer:

```c
uint8_t buf[WIDTH * HEIGHT];
gauge_frame_t frame = {
    .buf    = buf,
    .width  = WIDTH,
    .height = HEIGHT,
};
```

Use `GAUGE_SCRATCH_SIZE(width, height)` to declare the scratch buffer required by
calibration and blob extraction:

```c
size_t scratch[GAUGE_SCRATCH_SIZE(WIDTH, HEIGHT)];
```

### 2. Build background

Pass each frame from a static scene (no arrow movement) to build a per-pixel maximum
background. Start with `bg.buf` zeroed:

```c
uint8_t bg_buf[WIDTH * HEIGHT] = {0};
gauge_frame_t bg = {.buf = bg_buf, .width = WIDTH, .height = HEIGHT};

for (size_t i = 0; i < frame_count; i++) {
    gauge_err_t err = gauge_update_background(&frames[i], &bg);
    if (err != GAUGE_OK) { /* handle error */ }
}
```

### 3. Calibrate geometry

Provide the first frame (arrow at minimum position) and the last frame (arrow at
maximum position), along with the background:

```c
gauge_calibration_data_t ca_data;
gauge_err_t err = gauge_calibrate_by_axis_intersection(
    &first, &last, &bg, GAUGE_BINARIZATION_THRESHOLD,
    scratch, GAUGE_SCRATCH_SIZE(WIDTH, HEIGHT), &ca_data);
if (err != GAUGE_OK) { /* handle error */ }
```

`ca_data.spin` is `GAUGE_SPIN_UNKNOWN` at this point.

### 4. Determine spin direction

Pass a frame where the arrow has moved noticeably from its start position:

```c
gauge_err_t err = gauge_calibrate_spin(
    &frame, &bg, GAUGE_BINARIZATION_THRESHOLD,
    scratch, GAUGE_SCRATCH_SIZE(WIDTH, HEIGHT), &ca_data);
if (err == GAUGE_ERR_SPIN_UNDETERMINED) { /* arrow hasn't moved enough, try next frame */ }
```

Repeat with subsequent frames until `GAUGE_OK` is returned.

### 5. Measure

For each new frame, call:

```c
float angle;
gauge_err_t err = gauge_scan_radial(&frame, &ca_data, GAUGE_RADIAL_SCAN_STEP, &angle);
if (err != GAUGE_OK) { /* handle error */ }
```

Returns the arrow angle in radians, normalized to `[-π, π]`. Map it to a scale value
using your known angle-to-value calibration.

---

## Calibration methods

### `gauge_update_background`

Accepts frames one at a time and updates `bg` in-place via per-pixel maximization.
Call once per frame with `bg->buf` zeroed on the first call.

### `gauge_calibrate_by_axis_intersection`

**Algorithm:**

1. Subtract background from first and last frames — only the arrow remains.
2. Binarize each result using the provided threshold.
3. Extract the largest blob from each binarized frame.
4. Fit a line through each blob using principal component analysis.
5. Intersect the two lines to find the rotation pivot.
6. Compute the arrow length as the maximum pixel distance from pivot to blob.

`ca_data_out->spin` is set to `GAUGE_SPIN_UNKNOWN`; call `gauge_calibrate_spin`
afterward to determine direction.

### `gauge_calibrate_spin`

**Algorithm:**

1. Subtract background, binarize, extract the arrow blob.
2. Compute the arrow angle relative to `ca_data->pivot`.
3. If the angular difference from `ca_data->angle_start_rad` exceeds
   `GAUGE_CALIBRATE_SPIN_MIN_ANGLE_RAD` (~10°), write the spin direction and
   return `GAUGE_OK`. Otherwise return `GAUGE_ERR_SPIN_UNDETERMINED`.

---

## Measurement methods

### `gauge_scan_radial`

**Algorithm:**

Scans radially from `angle_start_rad` to `angle_end_rad` in steps of
`radial_scan_step`. At each angle, walks from the pivot along the radial direction
for `arrow_len` pixels, accumulating `255 - pixel_value` as a score. The angle with
the highest score is written to `angle_out`, normalized to `[-π, π]`.

**Default step:** `GAUGE_RADIAL_SCAN_STEP` (~0.5°). Smaller steps give higher angular
resolution at the cost of more computation.
