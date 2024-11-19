#include "render_common.h"
#include <stdio.h>

#include "oledm/font/terminus8x16.h"

static void _render_title(struct Bitmap* bm, const char* title) {
  bitmap_str(bm, terminus8x16, title, 0, 0, bitmap_SET);
  bitmap_hline(bm, 0, 17, 128, bitmap_SET);
}

static void _render_guide(
    struct Bitmap* bm, const char* prev_action, const char* next_action) {
  char str[32];
  // 16 total characters
  // 0123456789012345
  // <pppppp  nnnnnn>
  snprintf(str, sizeof(str), "<%-6s  %6s>", prev_action, next_action);
  bitmap_str(bm, terminus8x16, str, 0, 112, bitmap_SET);
  bitmap_hline(bm, 0, 110, 128, bitmap_SET);
}

void render_common(
    struct Bitmap* bm,
    const char* title,
    const char* prev_action,
    const char* next_action) {
  _render_title(bm, title);
  _render_guide(bm, prev_action, next_action);
}
