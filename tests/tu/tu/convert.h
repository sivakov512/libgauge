#pragma once

#include "gauge.h"
#include "utils.h"
#include <stddef.h>

/* Convert tu_image_t array (RGB) to gauge_frame_t array (grayscale).
 * Grayscale is written in-place into tu_image_t.buf; frames point into it.
 * For a single image pass count=1. */
void tu_to_frames(tu_image_t *imgs, gauge_frame_t *out, size_t count);

/* Convert gauge_frame_t (grayscale) to tu_image_t (RGB).
 * RGB is written into img->buf. */
void tu_from_frame(const gauge_frame_t *frame, tu_image_t *out);

/* Convert binarized gauge_frame_t values (0/1) to (0/255) in-place.
 * Allows saving a binarized frame as a readable image. */
void tu_unbinarize(gauge_frame_t *frame);
