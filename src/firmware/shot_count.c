#include "shot_count.h"

#include <stdio.h>

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"
#include "enter_value_widget.h"
#include "render_common.h"

struct EnterValueWidget count_evw;

static void reinit_count_evw(const struct MotorControl* motor_snap) {
  // a step of 1 is the smallest we can do
  int32_t max_shots = (int32_t)(motor_snap->current_pos);
  if (max_shots < 0) {
    max_shots = -max_shots;
  }
  count_evw.max_value = max_shots + 1;

  // go for about a 0.2mm step size by default
  // 0.25mm is 40 steps
  count_evw.value = (max_shots / 40) + 1;

  if (count_evw.value > count_evw.max_value) {
    count_evw.value = count_evw.max_value;
  }
}

static void update(
    struct SharedState* ss, const struct MotorControl* motor_snap) {
  if (count_evw.max_value == 0) {
    reinit_count_evw(motor_snap);
  }
  if (ss->button & NEXT_PRESSED) {
    ss->state = STATE_RUNNING;
  } else if (ss->button & PREVIOUS_PRESSED) {
    count_evw.max_value = 0;
    ss->state = STATE_SHUTTER_DELAY;
  } else {
    count_evw.value = ss->shot_count;
    enter_value_widget_update(&count_evw, -ss->gimbal_x_dir, -ss->gimbal_y_dir);
    ss->shot_count = count_evw.value;
  }
}

static void _render_image_count(struct Bitmap* bm, int16_t ypos) {
  bitmap_str(bm, terminus8x16, "Images:", 0, ypos, bitmap_SET);
  enter_value_widget_render(&count_evw, bm, 128, ypos, 0); 
}

static void _render_distance(
    struct Bitmap* bm, int16_t ypos, int32_t current_pos, uint32_t shot_count) {
  char str[32];
  int32_t dist_dm = 0;
  if (shot_count > 1) {
    dist_dm = current_pos / 2;
    if (dist_dm < 0) {
      dist_dm = -dist_dm;
    }
    dist_dm /= (shot_count - 1);
  }
  snprintf(
      str,
      sizeof(str),
      "Step:   %3d.%02dmm",
      dist_dm / 100,
      dist_dm % 100);
  bitmap_str(bm, terminus8x16, str, 0, ypos, bitmap_SET);
}

static void _render_time(
    struct Bitmap* bm, int16_t ypos, const struct SharedState* ss) {
  uint32_t time_secs = (
      (ss->shot_count * ss->shutter_delay_ms) +
      ((ss->shot_count - 1) * SETTLE_MS) + 999) / 1000;
  const int32_t hours = time_secs / 3600;
  time_secs -= (hours * 3600);
  const int32_t minutes = time_secs / 60;
  time_secs -= (minutes * 60);
  char str[32];
  snprintf(str, sizeof(str), "Time:  %02dh%02dm%02ds");
  bitmap_str(bm, terminus8x16, str, 0, ypos, bitmap_SET);
}

static void render(
    struct SharedState* ss,
    const struct MotorControl* motor_snap) {
  struct Bitmap* bm = &(ss->bitmap);
  render_common(bm, "Shot Count", "Delay", "START");
  int16_t ypos = 48;
  _render_image_count(bm, ypos);
  ypos += 16;
  _render_distance(bm, ypos, motor_snap->current_pos, ss->shot_count);
  ypos += 16;
  _render_time(bm, ypos, ss);
}

void shot_count_init(struct SharedState* ss) {
  ss->shot_count = 2;
  // zero is a special marked that this needs to be recalculated
  enter_value_widget_init(&count_evw, 2, 2, 0);
}

void shot_count_update(struct SharedState* ss) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));

  update(ss, &motor_snap);
  render(ss, &motor_snap);
}

