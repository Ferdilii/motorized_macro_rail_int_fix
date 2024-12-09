#include "saved_settings.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "logging.h"
#include <string.h>

#define FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_ADDRESS ((uint8_t*)(XIP_BASE + FLASH_OFFSET))

static struct SavedSettings settings;

// A simple checksum calculation that also incorporates
// SAVED_SETTINGS_VERSION.  If SAVED_SETTINGS_VERSION is changed
// then the checksum will fail (by design) until the data
// is rewritten with the updated version.
static uint32_t calc_checksum(const struct SavedSettings* cs) {
  uint8_t* start = ((uint8_t*)cs) + sizeof(uint32_t);
  size_t len = sizeof(struct SavedSettings) - sizeof(uint32_t);
  uint32_t sum = SAVED_SETTINGS_VERSION * 1000000;
  for (size_t i=0; i<len; ++i) {
    sum += start[i];
  }
  return sum;
}

// Looks at checksum and the eyecatcher to validate if the stored settings
// are good or bad.
static uint8_t validate_settings(void) {
  if ((settings.eyecatcher[0] != 'M') ||
      (settings.eyecatcher[1] != 'A') ||
      (settings.eyecatcher[2] != 'R') ||
      (settings.eyecatcher[3] != 'F')) {
    DEBUG_LOG("validate_settings: bad eyecatcher: %02x%02x%02x%02x\n",
    settings.eyecatcher[0],
    settings.eyecatcher[1],
    settings.eyecatcher[2],
    settings.eyecatcher[3]);

    return 0;
  }
  uint32_t checksum = calc_checksum(&settings);
  if (checksum != settings.checksum) {
    DEBUG_LOG("validate_settings: checksum calc=%08x flash=%08x\n",
    checksum, settings.checksum);
    return 0;
  }
  return 1;
}

static void init_default_settings(void) {
  DEBUG_LOG("init_default_settings\n");
  memset(&settings, 0, sizeof(struct SavedSettings));
  settings.version = SAVED_SETTINGS_VERSION;
  settings.max_velocity = DEFAULT_MAX_VELOCITY;
  settings.acceleration = DEFAULT_ACCELERATION;
  settings.backlash = DEFAULT_BACKLASH;
  settings.settle_dsecs = DEFAULT_SETTLE_DSECS;
  settings.steps_per_mm = DEFAULT_STEPS_PER_MM;
}

void saved_settings_init(void) {
  DEBUG_LOG("saved_settings_init\n");
  memcpy(&settings, FLASH_ADDRESS, sizeof(struct SavedSettings));
  if (!validate_settings()) {
    init_default_settings();
  }
  DEBUG_LOG("saved_settings_init: max_vel=%d\n", settings.max_velocity);
}


const struct SavedSettings* saved_settings_get(void) {
  return &settings;
}

static void saved_settings_write_internal(struct MotorControl* mc) {
  DEBUG_LOG("saved_settings_write_internal\n");
  uint8_t buff[FLASH_PAGE_SIZE];
  memset(buff, 0, sizeof(buff));
  memcpy(buff, &settings, sizeof(struct SavedSettings));
  struct SavedSettings* ss = (struct SavedSettings*)buff;
  ss->eyecatcher[0] = 'M';
  ss->eyecatcher[1] = 'A';
  ss->eyecatcher[2] = 'R';
  ss->eyecatcher[3] = 'F';
  ss->version = SAVED_SETTINGS_VERSION;
  ss->checksum = calc_checksum(&settings);
  DEBUG_LOG("saved_settings_write_internal: checksum=%08x max_vel=%d\n",
  ss->checksum, ss->max_velocity);
  motor_control_stop(mc);
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_OFFSET, buff, FLASH_PAGE_SIZE);
  restore_interrupts(ints);
  DEBUG_LOG("saved_settings_write_internal: done\n");
  motor_control_start_loop(mc);
}

void saved_settings_write(struct SharedState* state, const struct SavedSettings* ss) {
  memcpy(&settings, ss, sizeof(struct SavedSettings));
  saved_settings_write_internal(&(state->motor));
}

