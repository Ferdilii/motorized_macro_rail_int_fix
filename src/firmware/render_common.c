#include "render_common.h"

#include "oledm/font/terminus8x16.h"

void render_title(struct Bitmap* bm, const char* title) {
  bitmap_str(bm, terminus8x16, title, 0, 0, bitmap_SET);
  bitmap_hline(bm, 0, 17, 128, bitmap_SET);
}
