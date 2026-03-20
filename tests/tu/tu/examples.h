#pragma once

#include "gauge.h"
#include "utils.h"

/* Save image or frame as JPEG to examples_dir/name.jpg.
 * Side-effect helpers for visual inspection, not test assertions. */
void tu_examples_save_image(const char *examples_dir, const char *name,
                            const tu_image_t *img);

void tu_examples_save_frame(const char *examples_dir, const char *name,
                            const gauge_frame_t *frame);

#define EXAMPLES_SAVE_IMAGE(name, img)                                              \
    tu_examples_save_image(TEST_EXAMPLES_DIR, (name), (img))

#define EXAMPLES_SAVE_FRAME(name, frame)                                            \
    tu_examples_save_frame(TEST_EXAMPLES_DIR, (name), (frame))
