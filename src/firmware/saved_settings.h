// Contains program settings, stored in flash
#ifndef SAVED_SETTINGS_H
#define SAVED_SETTINGS_H

// This module handles settings.  It handles both the user interface
// (shell prompt over USB) and storing/loading from flash.

#include <inttypes.h>
#include "shared_state.h"

//Change this when adding fields.  It will cause all settings to reset
#define SAVED_SETTINGS_VERSION 1

// defaults
#define DEFAULT_MAX_VELOCITY 150
#define DEFAULT_ACCELERATION 150
#define DEFAULT_BACKLASH 25
#define DEFAULT_SETTLE_DSECS 10
#define DEFAULT_STEPS_PER_MM 200

// Here are the settings.  The checksum, eyecatcher and version fields
// are there to detect the cases where setting have not ever been written
// or were updated in an incompatible way.  Bad/unreliable flash could also
// cause mismatches here. Having the eyscatcher is probably caution overkill
// but it can help in troubleshooting efforts.
struct SavedSettings {
  uint32_t checksum;  // the sum of all byte in this structure except for this one
  uint8_t eyecatcher[4];  // 'MARF'
  uint32_t version;  // bump this when structure changes

  int32_t max_velocity;
  int32_t acceleration;
  int32_t backlash;
  int32_t settle_dsecs;  // seconds * 10
  int32_t steps_per_mm;
};

void saved_settings_init(void);

// Get a read-only copy of the settings.
const struct SavedSettings* saved_settings_get(void);

// Update saved settings.  Note this writes to flash which causes wear,
// thus should only be done occasionally.
// This also disables interrupts (as previous usage of flash write have
// deemed this necessary).
void saved_settings_write(
    struct SharedState* state, const struct SavedSettings* ss);

#endif
