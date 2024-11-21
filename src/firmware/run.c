#include "run.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdio.h>

#include "fatal.h"
#include "render_common.h"
#include "shutter.h"
#include "oledm/font/terminus8x16.h"

#define SHUTTER_RELEASE_MS 20

#define RUN_STATE_TAKE_PHOTO 0
#define RUN_STATE_SHUTTER_WAIT 1
#define RUN_STATE_WAIT_MOTOR_STOP 2
#define RUN_STATE_WAIT_SETTLE 3
#define RUN_STATE_FINISHED 4

struct RunState {
  uint32_t shot_idx;
  uint32_t wait_end_ms;
  uint32_t started_ms;
  uint32_t ended_ms;
  uint8_t state;
  uint8_t paused;
} rs;

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

void run_init(void) {
  memset(&rs, 0, sizeof(rs));
  rs.started_ms = uptime_ms();
}

static void _take_photo(uint32_t shutter_delay_ms) {
  rs.wait_end_ms = uptime_ms() + shutter_delay_ms;
  shutter_release(SHUTTER_RELEASE_MS);
  rs.state = RUN_STATE_SHUTTER_WAIT;
}

static void _shutter_wait(struct SharedState* ss) {
  if (uptime_ms() < rs.wait_end_ms) {
    return;
  }

  ++rs.shot_idx;

  if (rs.shot_idx >= ss->shot_count) {
    // Done
    rs.state = RUN_STATE_FINISHED;
    rs.ended_ms = uptime_ms();
    --rs.shot_idx;
    return;
  }

  // it should end at zero on the last shot
  int32_t new_pos = ss->start_pos - ((int32_t)rs.shot_idx * ss->start_pos / ((int32_t)ss->shot_count - 1));

  if (motor_control_try_target_position(&(ss->motor), new_pos)) {
    rs.state = RUN_STATE_WAIT_MOTOR_STOP;
  } else {
    // it didn't work.  Try again later
    --rs.shot_idx;
  }
}

static void _wait_motor_stop(struct SharedState* ss) {
  if (!motor_control_check_stopped(&(ss->motor))) {
    // not there yet
    return;
  }
  rs.wait_end_ms = uptime_ms() + SETTLE_MS;
  rs.state = RUN_STATE_WAIT_SETTLE;
}

static void _wait_settle() {
  if (uptime_ms() >= rs.wait_end_ms) {
    rs.state = RUN_STATE_TAKE_PHOTO; 
  }
}

void _update_state(struct SharedState* ss, const struct MotorControl* motor_snap) {
  if (ss->button & NEXT_PRESSED) {
    if (rs.state == RUN_STATE_FINISHED) {
      // Here we set for a reversal.  Current pos is expected to be at zero.
      // we change it to -start_pos so that the new zero is where the old one
      // started.  Example:
      // The user has start_pos at 10 and ran through.
      // now the pos is zero.
      // change the pos to -10 so that zero is the orignal 10.
      if (!motor_control_try_set_current_pos(&(ss->motor), -ss->start_pos)) {
        // failed to work
        return;
      }
      ss->start_pos = -ss->start_pos;
      ss->state = STATE_SHOT_COUNT;
      return;
    }
    rs.paused = 1 - rs.paused;
  } else if (ss->button & PREVIOUS_PRESSED) {
    motor_control_set_jog_velocity(&(ss->motor), 0);
    ss->state = STATE_END_POINT_SELECT;
  }

  if (rs.paused) {
    return;
  }

  switch (rs.state) {
    case RUN_STATE_TAKE_PHOTO:
      // take a photo right away
      _take_photo(ss->shutter_delay_ms);
      break;
    case RUN_STATE_SHUTTER_WAIT:
      _shutter_wait(ss);
      break;
    case RUN_STATE_WAIT_MOTOR_STOP:
      _wait_motor_stop(ss);
      break;
    case RUN_STATE_WAIT_SETTLE:
      _wait_settle();
      break;
    case RUN_STATE_FINISHED:
      break;
    default:
      fatal(ss, "Unknown run state: %d", rs.state);
  }
}

void _render_title(struct SharedState* ss) {
  struct Bitmap* bm = &(ss->bitmap);
  const char* state_str = "Run: Wait Photo";
  const char* prev_str = "Cancel";
  const char* next_str = "Pause";
  switch (rs.state) {
    case RUN_STATE_WAIT_MOTOR_STOP:
      state_str = "Run: Move Camera";
      break;
    case RUN_STATE_WAIT_SETTLE:
      state_str = "Run: Wait Settle";
      break;
    case RUN_STATE_FINISHED:
      state_str = "Run: Finished";
      prev_str = "Reset";
      next_str = "Revers";
      break;
  }

  if (rs.paused) {
    state_str = "Run: Paused";
    next_str = "Resume";
  }

  render_common(bm, state_str, prev_str, next_str);
}

void _time_append(char* dest, uint32_t time_secs) {
  const int32_t hours = time_secs / 3600;
  time_secs -= (hours * 3600);
  const int32_t minutes = time_secs / 60;
  time_secs -= (minutes * 60);
  char str[16];
  sprintf(
      str,
      "%02dh%02dm%02ds",
      hours,
      minutes,
      time_secs);
  strcat(dest, str);
}

void _elapsed_str_append(char* str) {
  uint32_t end_time_ms = rs.state == RUN_STATE_FINISHED ? rs.ended_ms : uptime_ms();
  uint32_t elapsed_sec = (end_time_ms - rs.started_ms) / 1000;
  _time_append(str, elapsed_sec);
}

void _total_str_append(const struct SharedState* ss, char* str) {
  uint32_t time_secs = (
      (ss->shot_count * ss->shutter_delay_ms) +
      ((ss->shot_count - 1) * SETTLE_MS) + 999) / 1000;
  _time_append(str, time_secs);
}

void _render(struct SharedState* ss, const struct MotorControl* motor_snap) {
  struct Bitmap* bm = &(ss->bitmap);
  char str[32];
  _render_title(ss);

  const float pos_mm = motor_snap->current_pos / 200.0;
  int16_t ypos = 32;
  snprintf(str, sizeof(str), "Pos:   %5.2fmm", pos_mm);
  bitmap_str(bm, terminus8x16, str, 0, ypos, bitmap_SET);
  ypos += 16;
  snprintf(str, sizeof(str), "Shot: %4d/%4d", rs.shot_idx + 1, ss->shot_count);
  bitmap_str(bm, terminus8x16, str, 0, ypos, bitmap_SET);
  ypos += 16;
  strcpy(str, "Elapsed:");
  _elapsed_str_append(str);
  bitmap_str(bm, terminus8x16, str, 0, ypos, bitmap_SET);
  ypos += 16;
  strcpy(str, "Total:  ");
  _total_str_append(ss, str);
  bitmap_str(bm, terminus8x16, str, 0, ypos, bitmap_SET);
}

void run_update(struct SharedState* ss) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));
  _update_state(ss, &motor_snap);
  _render(ss, &motor_snap);
}


