#include "shot_count.h"

#include <stdio.h>

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"
#include "enter_value_widget.h"
#include "render_common.h"
#include "run.h"
#include "saved_settings.h"

struct EnterValueWidget count_evw;

static void reinit_count_evw(
    struct SharedState* ss, const struct MotorControl* motor_snap) {
  // a step of 1 is the smallest we can do
  int32_t max_shots = (int32_t)(motor_snap->current_pos);
  if (max_shots < 0) {
    max_shots = -max_shots;
  }
  count_evw.max_value = max_shots + 1;

  // go for about a 0.2mm step size by default
  // 0.2mm is 40 steps
  const struct SavedSettings* settings = saved_settings_get();
  ss->shot_count = (max_shots * 5 / settings->steps_per_mm) + 1;
}

static void update(
    struct SharedState* ss, const struct MotorControl* motor_snap) {
  if (count_evw.max_value == 0) {
    reinit_count_evw(ss, motor_snap);
  }
  if (ss->button & NEXT_PRESSED) {
    run_init();
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
  bitmap_str(bm, TEXT_FONT, "Images:", 0, ypos, bitmap_SET);
  enter_value_widget_render(&count_evw, bm, 128, ypos, 0); 
}

static void _render_distance(
    struct Bitmap* bm,
    int16_t ypos,
    int32_t current_pos,
    int32_t shot_count,
    int32_t steps_per_mm) {
  char str[32];
  float dist_mm = 0;
  if (shot_count > 1) {
    dist_mm = (float)current_pos / (float)steps_per_mm;

    snprintf(str, sizeof(str), "Dist:  %5.1fmm", dist_mm);
    bitmap_str(bm, TEXT_FONT, str, 0, ypos, bitmap_SET);
    ypos += TEXT_HEIGHT;
    dist_mm /= (shot_count - 1);
  }
  if (dist_mm < 0) {
    dist_mm = -dist_mm;
  }
  snprintf(str, sizeof(str), "Step:  %3.3fmm", dist_mm);
  bitmap_str(bm, TEXT_FONT, str, 0, ypos, bitmap_SET);
}

static inline uint32_t settle_ms(void) {
  const struct SavedSettings* settings = saved_settings_get();
  return settings->settle_dsecs * 100;
}

static void _render_time(
    struct Bitmap* bm,
    int16_t ypos,
    const struct SharedState* ss,
    const struct MotorControl* motor_snap) {
  uint32_t time_secs = (ss->shot_count * ss->shutter_delay_ms +
      (ss->shot_count - 1) * (settle_ms() + estimate_seek_time_ms(motor_snap, ss->start_pos)) + 999)
      / 1000;
  const int32_t hours = time_secs / 3600;
  time_secs -= (hours * 3600);
  const int32_t minutes = time_secs / 60;
  time_secs -= (minutes * 60);
  char str[32];
  snprintf(
      str,
      sizeof(str),
      "Time:  %02dh%02dm%02ds",
      hours,
      minutes,
      time_secs);
  bitmap_str(bm, TEXT_FONT, str, 0, ypos, bitmap_SET);
}

static void render(
    struct SharedState* ss,
    const struct MotorControl* motor_snap) {
  struct Bitmap* bm = &(ss->bitmap);
  render_common(bm, "Shot Count", "Delay", "START");
  int16_t ypos = TEXT_HEIGHT * 2;
  _render_image_count(bm, ypos);
  ypos += TEXT_HEIGHT;
  const struct SavedSettings* settings = saved_settings_get();
  _render_distance(
      bm,
      ypos,
      motor_snap->current_pos,
      ss->shot_count,
      settings->steps_per_mm);
  ypos += TEXT_HEIGHT * 2;
  _render_time(bm, ypos, ss, motor_snap);
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

