# libgauge

[![Checks](https://github.com/sivakov512/libgauge/actions/workflows/checks.yml/badge.svg)](https://github.com/sivakov512/libgauge/actions/workflows/checks.yml)
[![Formatting](https://github.com/sivakov512/libgauge/actions/workflows/formatting.yml/badge.svg)](https://github.com/sivakov512/libgauge/actions/workflows/formatting.yml)

C library for reading analog gauge values from video frames. Designed for constrained environments — no OpenCV, no heavy dependencies. Works on embedded systems with a camera.

## Requirements

- CMake 3.16+
- C99 compiler
- `libm` (standard math library)

## Integration

```cmake
include(FetchContent)

FetchContent_Declare(
    gauge
    GIT_REPOSITORY https://github.com/sivakov512/libgauge.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(gauge)

target_link_libraries(your_target PRIVATE gauge)
```

Include the headers you need:

```c
#include "gauge.h"      // calibration and measurement
#include "gauge/cv.h"   // computer vision primitives
#include "gauge/utils.h" // angle utilities
```

## Usage

### 1. Prepare frames

Frames are grayscale, row-major. You allocate the buffer:

```c
uint8_t buf[WIDTH * HEIGHT];
gauge_frame_t frame = {
    .buf = buf,
    .buf_len = sizeof(buf),
    .width = WIDTH,
    .height = HEIGHT,
};
```

### 2. Calibrate

Capture a sequence of frames while the arrow moves through its full range (minimum to maximum scale position). Pass them to the calibration function:

```c
gauge_calibration_data_t ca_data;
gauge_err_t err = gauge_calibrate_by_axis_intersection(
    frames, frames_len, GAUGE_BINARIZATION_THRESHOLD, &ca_data);
if (err != GAUGE_OK) { /* handle error */ }
```

Store `ca_data` — it describes the gauge geometry and is reused for every measurement.

### 3. Measure

For each new frame, call:

```c
float angle = gauge_scan_radial(&frame, &ca_data, GAUGE_RADIAL_SCAN_STEP);
```

Returns the arrow angle in radians, normalized to `[-π, π]`. Map it to a scale value using your known angle-to-value calibration.

---

## Calibration methods

### `gauge_calibrate_by_axis_intersection`

**Algorithm:**

1. Compute background as the per-pixel maximum across all frames.
2. Subtract background from the first and last frames — only the arrow remains.
3. Binarize each result using the provided threshold.
4. Extract the largest blob from each binarized frame.
5. Fit a line through each blob using principal component analysis.
6. Intersect the two lines to find the rotation pivot.
7. Determine spin direction by tracking the arrow across intermediate frames.
8. Compute the arrow length as the maximum pixel distance from pivot to blob.

**Requirements:** at least 3 frames; the arrow must cover its full range between the first and last frame.

---

## Measurement methods

### `gauge_scan_radial`

**Algorithm:**

Scans radially from `angle_start_rad` to `angle_end_rad` in steps of `radial_scan_step`. At each angle, walks from the pivot along the radial direction for `arrow_len` pixels, accumulating `255 - pixel_value` as a score. The angle with the highest score is returned, normalized to `[-π, π]`.

**Default step:** `GAUGE_RADIAL_SCAN_STEP` (~0.5°). Smaller steps give higher angular resolution at the cost of more computation.
