#include "tu/examples.h"
#include "tu/convert.h"
#include "tu/image.h"
#include <limits.h>
#include <stdio.h>

void tu_examples_save_image(const char *examples_dir, const char *name,
                            const tu_image_t *img) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s.jpg", examples_dir, name);
    (void) tu_image_save(path, img);
}

void tu_examples_save_frame(const char *examples_dir, const char *name,
                            const gauge_frame_t *frame) {
    tu_image_t img;
    tu_from_frame(frame, &img);
    tu_examples_save_image(examples_dir, name, &img);
}
