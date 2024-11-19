#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#include "oledm/bitmap.h"

// renders a common overlay
// 16 chars max for title
// 6 chars max for prev_action and next_action
void render_common(
    struct Bitmap* bm,
    const char* title,
    const char* prev_action,
    const char* next_action);

#endif
